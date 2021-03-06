/* Filename: cset.h
 * ----------------
 * Implementation of the set data type in C. This header file provides information about
 * the data structure for the client's use. For information about the implementation itself,
 * refer to the .c file.
 *
 * A set is a collection of distinct objects. This implementation requires the client to provide
 * a size for each element, a hint of how large the set will be, a comparator function for the elements,
 * a cleanup function for elements if they are heap-allocated, and an optional toString function to
 * allow printing. In order for the set to work correctly, all elements in a set must have the same type.
 *
 * One key piece of functionality that the CSet offers is nested sets. Comparator, cleanup, and toString
 * functions are provided so that sets of sets can be created. 
 */

#ifndef _cset_h
#define _cset_h

#include <stdbool.h>    //for bool
#include <stdlib.h>     //for size_t

    /* * * * * Type Definitions * * * * */

/* Type Definition: CompareFn
 * --------------------------
 * Definition of a generic void* comparator function. Used to compare two values stored at
 * addresses addr1 and addr2. A typical comparator dereferences and casts the pointers as
 * a given type and uses some comparison on them.
 * Returns: a positive number if value at addr1 > value at addr2, 0 is the values are equal,
 * and a negative number if value at addr1 < value at addr2.
 */ 
typedef int (*CompareFn)(const void* addr1, const void* addr2);

/* Type Definition: CleanupElemFn
 * ------------------------------
 * Definition of a generic void* cleanup function. Frees all heap-allocated memory associated
 * with the value at addr. Typically involves deferencing and casting the pointer and then calling
 * free or another memory-freeing function  on the element. Called in cset_delete.
 */ 
typedef void (*CleanupElemFn)(void* addr);

/* Type Definition: ToStringFn
 * ---------------------------
 * Definition of a generic void* toString function. Should return a heap-allocated string representation
 * of the element contained at address addr. Client is responsible for freeing any strings generated by
 * the toString function.
 */ 
typedef char* (*ToStringFn)(const void* addr);

/* Incomplete Type Definition: CSet
 * --------------------------------
 * Defines the CSet type. The implementation remains opaque to the client for simplicity. A client should
 * not be accessing the fields of the CSet directly, but instead calls methods on CSet pointers (CSet*).
 */
typedef struct CSetImplementation CSet;

    /* * * * * Public Functions * * * * */

/* Function: cset_create
 * ---------------------
 * Creates a CSet using the given fields and returns a pointer to it. elemsz defines the size of each element stored in
 * the set. capacity_hint is an estimate of how many elements will be stored in the set. If unknown, 0 can be passed and
 * a default initial value will be used. The capacity_hint is not a hard limit to the number of elements that can be stored
 * in it--if needed, the set will increase its capacity by a predetermined factor, but performance may be impacted.
 * 
 * The client must also pass a comparator function (cmp_fn). The set's internal storage of the elements is ordered, and this
 * function is used to determine that ordering. It is essential that cmp_fn is a valid comparator function. The client can
 * also pass an optional cleanup function (cleanup_fn) if the elements of the set involve heap-allocated memory. The client
 * also has the option of passing a toString function (toString_fn), which enables printing of the set's elements.
 */ 
CSet* cset_create(size_t elemsz, size_t capacity_hint, CompareFn cmp_fn, CleanupElemFn cleanup_fn, ToStringFn toString_fn);

/* Function: cset_delete
 * ---------------------
 * Frees all heap-allocated memory associated with the given set and deletes the set itself. Calls the client's cleanup function
 * on each element in the set if that cleanup function is non-NULL.
 */ 
void cset_delete(CSet* set);

/* Function: cset_add
 * ------------------
 * Adds the element at address elem to the givenn set. The element itself is copied and stored in the CSet, rather than a pointer
 * to the element. 
 */ 
bool cset_add(CSet* set, void* elem);

/* Function: cset_clear
 * --------------------
 * Removes all elements from the CSet, freeing all heap-allocated memory associated with them. Equivalent to calling cset_remove
 * on every element, but vastly more efficient.
 */ 
void cset_clear(CSet* set);

/* Function: cset_contains
 * -----------------------
 * Returns true if the element at address elem is contained in the given set, or false if not. Uses the client's comparator function
 * given at the creation of set to search for the given element.
 */ 
bool cset_contains(CSet* set, void* elem);

/* Function: cset_remove
 * ---------------------
 * Removes the element at address elem from the given set if it is contained in the set. Calls the client's cleanup function on
 * the element. Returns true if the element was successfully removed, or false if the element was not found.
 */ 
bool cset_remove(CSet* set, void* elem);

/* Functions: cset_size, cset_cardinality
 * --------------------------------------
 * Returns the number of elements in the given set. The two functions are interchangable.
 */ 
int cset_size(CSet* set);
int cset_cardinality(CSet* set);

/* Function: cset_isEmpty
 * ----------------------
 * Returns true if the given set contains 0 elements. Returns false otherwise.
 */ 
bool cset_isEmpty(CSet* set);

/* Function: cset_isSubsetOf
 * -------------------------
 * Returns true if set1 is a subset of set2, i.e. if set2 contains all of the elements
 * in set1. Membership is determined using the client's comparator function. Returns false
 * if set1 is not a subset of set2.
 */ 
bool cset_isSubsetOf(CSet* set1, CSet* set2);

/* Function: cset_union
 * --------------------
 * Returns a new heap-allocated set containing all elements contained in either set1 or set2. The new
 * set is initialzed using the comparator, cleanup, and toString functions of set1. Thus, set1 and set2
 * must have the same type in order for union to function correctly.
 */ 
CSet* cset_union(CSet* set1, CSet* set2);

/* Function: cset_intersect
 * ------------------------
 * Returns a new heap-allocated set containing all elements contained in both set1 and set2. The new
 * set is initialized using the comparator, cleanup, and toString functions of set1. Thus, set1 and set2
 * must have the same type in order for union to function correctly.
 */ 
CSet* cset_intersect(CSet* set1, CSet* set2);

/* Function: cset_difference
 * -------------------------
 * Returns a new heap-allocated set containing all elements contained in set1 but not in set2. In set
 * notation, returns set1 - set2. The new set is initialized using the comparator, cleanup, and toString
 * functions of set1. Thus, set1 and set2 must have the same type in order for union to function correctly.
 */ 
CSet* cset_difference(CSet* set1, CSet* set2);

/* Function: cset_symmetricDifference
 * ----------------------------------
 * Returns the symmetric difference of set1 and set2, which is equivalent to the union of (set1 - set2) and 
 * (set2 - set1). As with the above, set1 and set2 must be store the same type.
 */ 
CSet* cset_symmetricDifference(CSet* set1, CSet* set2);

/* Function: cset_powerSet
 * -----------------------
 * Returns the power set of the given set, which is a set containing all subsets of the given set. The power
 * set is a new heap-allocated set of type set, initialized using the functions for CSets given below.
 */ 
CSet* cset_powerSet(CSet* set);

/* Functions: cset_first, cset_next
 * --------------------------------
 * These functions are iterators over the set. cset_first returns a pointer to the first (or "least" by
 * the client's comparator function) element in the set, or NULL if the set is empty. cset_next returns,
 * given the previous element, the next element in the set, or NULL if the end has been reached. Allows
 * looping over the elements in the set.
 */ 
void* cset_first(CSet* set);
void* cset_next(CSet* set, void* prev);

/* Function: cset_toString
 * -----------------------
 * Returns a heap-allocated string representation of the given set of the following form:
 *      {elem1, elem2, elem3, ... , elemN}
 * where N is the number of elements in the set and the elements are ordered least to greatest.
 */ 
char* cset_toString(CSet* set);

/* Functions: cset_compare, cset_cleanup, cset_genericToString
 * -----------------------------------------------------------
 * Comparator, cleanup, and toString function for sets, provided to allow functionality for nested
 * sets. If creating a set of type CSet*, use these functions to initialize the set. Conform
 * to the definitions of CompareFn, CleanupElemFn, and ToStringFn.
 */ 
int cset_compare(const void* addr1, const void* addr2);
void cset_cleanup(void* addr);
char* cset_genericToString(const void* addr);

#endif
