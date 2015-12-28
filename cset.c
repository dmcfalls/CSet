/* Filename: cset.c
 * ----------------
 * Implemenation of the set datatype in C as an ordered array. This implementation is
 * unusual because it keeps the elements ordered, which is not a requirement of a set.
 * Keeping the elements ordered allows the internal array to be searched with a binary
 * search, which is drastically better than a linear search. It also improves the
 * efficiency of the iterators, so the elements of the set can be given in order.
 *
 * The set operates with a void* interface, allowing the client to store any type of
 * object in the set as long as a valid comparator and cleanup function are supplied.
 * This means that nesting collections within the set requires the client to supply these
 * functions with the collections.
 *
 * The set is designed to store elements of the same size, but functions according to the
 * functions supplied by the client. This makes storage of multiple types possible provided
 * there is some sort of tagging system in place to be used by the comparator function.
 */ 

#include "cset.h"   //Includes stdbool.h and stdlib.h
#include <assert.h>
#include <string.h>
#include <stdio.h>


    /* * * * * Constant Definitions * * * * */

//Default values for recommended initial capacity and resizing factor.
#define DEFAULT_CAPACITY 32
#define RESIZE_FACTOR 2

//Maximum length of a toString representation of a set. Needs to be very high to accommodate printing power sets.
#define SET_STR_MAX_LEN 2000

    /* * * * * Struct Definitions * * * * */


/* Type Definition: CSet
 * ---------------------
 * The CSet is implemented as an ordered void* array.
 */ 
struct CSetImplementation {
    void* elements;
    int n_elements;
    size_t elemsz;
    size_t capacity;
    CompareFn cmp_fn;
    CleanupElemFn cleanup_fn;
    ToStringFn toString_fn;
};

    /* * * * * Private Helper Functions * * * * */

/* Function: nth
 * -------------
 * Returns the nth element of the elements array of the given set.
 */ 
static inline void* nth(CSet* set, int n) {
    return (char *)set->elements + (n * set->elemsz);
}

/* Function: get_index
 * -------------------
 * Returns the index of the elem in the elements array of the given set.
 */ 
static inline int get_index(CSet* set, void* elem) {
    return ((char *)elem - (char *)set->elements) / set->elemsz;
}

/* Function: check_resize
 * ----------------------
 * Checks to see if the set's element array needs to be resized and resizes it if needed.
 */
static inline void check_resize(CSet* set) {
    if(set->n_elements >= (set->capacity - 1)) {
        size_t new_capacity = set->capacity * RESIZE_FACTOR;
        set->capacity = new_capacity;
        set->elements = realloc(set->elements, set->elemsz * new_capacity);
    }
}

/* Function: insert
 * ----------------
 * Inserts the given element into the set's elements array at the given index. Used by cset_add.
 */
static inline void insert(CSet* set, void* elem, int index) {
    size_t elems_left = set->n_elements - index;
    void* elem_dest = nth(set, index);
    if(elems_left > 0) {
        void* rest_dest = nth(set, index + 1);
        memmove(rest_dest, elem_dest, set->elemsz * elems_left);
    }
    memcpy(elem_dest, elem, set->elemsz);
}


    /* * * * * Public Member Functions * * * * */


/* Function: cset_create
 * ---------------------
 * Allocates memory for a set and initializes its fields using the client-supplied values/functions.
 */ 
CSet* cset_create(size_t elemsz, size_t capacity_hint, CompareFn cmp_fn, CleanupElemFn cleanup_fn, ToStringFn toString_fn) {
    //Ensures that comparator function is non-null. Comparator must be valid for this implementation to work.
    assert(cmp_fn != NULL);
    //Assigns the initial capacity to a predetermined default if 0 is passed as capacity_hint.
    size_t capacity = capacity_hint == 0 ? DEFAULT_CAPACITY : capacity_hint;

    CSet* set = malloc(sizeof(CSet));
    set->elements = malloc(elemsz * capacity);

    assert(set != NULL && set->elements != NULL);

    set->n_elements = 0;
    set->elemsz = elemsz;
    set->capacity = capacity;
    set->cmp_fn = cmp_fn;
    set->cleanup_fn = cleanup_fn;
    set->toString_fn = toString_fn;

    return set;
}

/* Function: cset_delete
 * ---------------------
 * Frees all memory associated with a set, calling the client's cleanup function if it exists.
 */ 
void cset_delete(CSet* set) {
    //If the client has supplied a cleanup function, calls it on each element of the set.
    if(set->cleanup_fn != NULL) {
        for(int i = 0; i < set->n_elements; i++) {
            set->cleanup_fn(nth(set, i));
        }
    }
    //Frees the elements array and the set struct itself.
    free(set->elements);
    free(set);
}

/* Function: cset_add
 * ------------------
 * Adds an element to the given set. If the set already contains that element, returns false to indicate that nothing
 * was added. Uses a binay searching algorithm to find the index where the element should be inserted. Resizes the
 * elements array if necessary and increments the element count.
 */ 
bool cset_add(CSet* set, void* elem) {
    //Uses a binary searching algorithm to find where elem should go in the array, then inserts it.
    int partial_sz = set->n_elements;
    int index = partial_sz / 2;
    int cmp_result = 0;
    while(partial_sz > 1) {
        cmp_result = set->cmp_fn(elem, nth(set, index));
        partial_sz /= 2;
        //If the set already contains the given element, does nothing and returns false.
        if(cmp_result == 0) return false;
        //If elem is greater than the item at index, searches upper part of array. Otherwise, searches lower part.
        if(cmp_result > 0) {
            index += partial_sz / 2;
        } else {
            index -= partial_sz / 2;
        }
    }
    //A brief linear search at the end to catch any problems from integer division (index always conservatively low after above search).
    if(index == 1) index = 0;
    while(index < set->n_elements) {
        if(set->cmp_fn(elem, nth(set, index)) > 0) index++;
        else break;
    }
    //Checks for resize, inserts the element, and increases the element count of the set.
    check_resize(set);
    insert(set, elem, index);
    (set->n_elements)++;
    return true;
}

/* Function: cset_clear
 * --------------------
 * Removes all elements from the set and returns the element count to zero. Does not alter the capacity.
 */ 
void cset_clear(CSet* set) {
    if(set->cleanup_fn != NULL) {
        for(int i = 0; i < set->n_elements; i++) {
            set->cleanup_fn(nth(set, i));
        }
    }
    set->n_elements = 0;
}

/* Function: cset_contains
 * -----------------------
 * Since the elements of the set are stored in an ordered array, we can use binary search
 * to determine whether the given element is in a set.
 */
bool cset_contains(CSet* set, void* elem) {
    return bsearch(elem, set->elements, set->n_elements, set->elemsz, set->cmp_fn) != NULL;
}

/* Function: cset_remove
 * ---------------------
 * Uses binary search to find elem in the given set. If elem is not found, returns false. If found,
 * uses memmove to overwrite elem and decrease the size of the array, decrements element count,
 * and return true.
 */ 
bool cset_remove(CSet* set, void* elem) {
    void* found = bsearch(elem, set->elements, set->n_elements, set->elemsz, set->cmp_fn);
    if(found == NULL) return false;
    
    //Calculates distance between found and base of array.
    int index = get_index(set, found);
    int bytes_left = (set->n_elements - index) * set->elemsz;
    memmove(found, nth(set, index + 1), bytes_left);
    (set->n_elements)--;
    return true;
}

/* Function: cset_size
 * -------------------
 * Returns the number of elements in the set.
 */ 
int cset_size(CSet* set) {
    return set->n_elements;
}

/* Function: cset_cardinality
 * --------------------------
 * An alias for cset_size using set terminology.
 */ 
int cset_cardinality(CSet* set) {
    return cset_size(set);
}

/* Function: cset_isEmpty
 * ----------------------
 * Returns true if the set has no elements (i.e. is the empty set) and false otherwise.
 */ 
bool cset_isEmpty(CSet* set) {
    return set->n_elements == 0;
}

/* Function: cset_isSubsetOf
 * -------------------------
 * Returns true if set1 is a subset of set2, i.e. if set2 contains all of the elements that are in set2.
 * Comparison is judged based on set1's cmp function.
 */ 
bool cset_isSubsetOf(CSet* set1, CSet* set2) {
    //Optimization: if set1 and set2 are the same, then set1 must be a subset of set2.
    if(set1 == set2) return true;

    for(int i = 0; i < set1->n_elements; i++) {
        if(!cset_contains(set2, nth(set1, i))) return false;
    }
    return true;
}

/* Function: cset_union
 * --------------------
 * Returns the union of set1 and set2: a set containing every element from each set. Always
 * creates (and allocates memory for) a new set rather than pointing back to one of the two
 * if one is a subset of the other. Returns NULL if either set1 or set2 are NULL.
 * Raises an assert if set1 and set2 do not have the same elemsz. Uses set1's cmp function,
 * cleanup function, and toString function. Thus, set1 and set2 must have the same type.
 */ 
CSet* cset_union(CSet* set1, CSet* set2) {
    if(set1 == NULL || set2 == NULL) return NULL;
    assert(set1->elemsz == set2->elemsz);

    CSet* u = cset_create(set1->elemsz, set1->capacity, set1->cmp_fn, set1->cleanup_fn, set1->toString_fn);

    for(int i = 0; i < set1->n_elements; i++) {
        cset_add(u, nth(set1, i));
    }

    for(int i = 0; i < set2->n_elements; i++) {
        cset_add(u, nth(set2, i));
    }

    return u;
}

/* Function: cset_intersect
 * ------------------------
 * Returns the intersect of set1 and set2: a set containing all elements that appear in both sets.
 * Always creates (and allocates memory for) a new set. Returns NULL if either set1 or set2 are NULL
 * and asserts that they have the same elemsz. Uses set1's cmp, cleanup, and toString functions.
 * Precondition: set1 and set2 store the same type of elements.
 */ 
CSet* cset_intersect(CSet* set1, CSet* set2) {
    if(set1 == NULL || set2 == NULL) return NULL;
    assert(set1->elemsz == set2->elemsz);

    CSet* intersect = cset_create(set1->elemsz, set1->capacity, set1->cmp_fn, set1->cleanup_fn, set1->toString_fn);
    
    //Traverses the elements array of the smaller set and adds elements-in-common with set2 to the intersect set. 
    int smaller_sz = set1->n_elements < set2->n_elements ? set1->n_elements : set2->n_elements;
    for(int i = 0; i < smaller_sz; i++) {
        void* ith = nth(set1, i);
        if(cset_contains(set2, ith)) cset_add(intersect, ith);
    }

    return intersect;
}

/* Function: cset_difference
 * -------------------------
 * Computes the set difference of set1 and set2, i.e. set1 - set2, which is a set
 * of all elements that appear in set1 but not in set2. Returns NULL if either set1 or set2 are NULL
 * and asserts that they have the same elemsz. Uses set1's cmp, cleanup, and toString functions.
 */ 
CSet* cset_difference(CSet* set1, CSet* set2) {
    if(set1 == NULL || set2 == NULL) return NULL;
    assert(set1->elemsz == set2->elemsz);

    CSet* diff = cset_create(set1->elemsz, set1->capacity, set1->cmp_fn, set1->cleanup_fn, set1->toString_fn);

    for(int i = 0; i < set1->n_elements; i++) {
        if(!cset_contains(set2, nth(set1, i))) cset_add(diff, nth(set1, i));
    }

    return diff;
}

/* Function: cset_symmetricDifference
 * ----------------------------------
 * Returns the symmetric difference of set1 and set2, which is just (set1 - set2) u (set2 - set1).
 */ 
CSet* cset_symmetricDifference(CSet* set1, CSet* set2) {
    CSet* diff1 = cset_difference(set1, set2);
    CSet* diff2 = cset_difference(set2, set1);
    CSet* symm_diff = cset_union(diff1, diff2);

    cset_delete(diff1);
    cset_delete(diff2);

    return symm_diff;
}

/* Function: cset_powerSet
 * -----------------------
 * Returns a set containing all the subsets of the passed set. The returned set will be a new set
 * containing elements of type CSet*. Generates all subsets of a given set by using a bit vector to
 * represent which elements to choose for a given subset. Performs this operation for every bit vector
 * from 1 to the size of the set.
 *
 * For example, a bit vector of 0000 0101 would correspond to choosing the elements at indices 0 and 2, and
 * a bit vector of 0001 0110 would correspond to choosing elements 1, 2, and 4. This algorithm insures that all
 * possible subsets are generated. Using the popcount of the bit vector as the size of the subset perfectly
 * sizes the subset's initial capacity. The empty set is optimized out of the loop b/c a capacity hint of 0
 * would trigger the default capacity, and we want to minimize its size to 1.
 */ 
CSet* cset_powerSet(CSet* set) {
    //Uses 2^n where n is the size of the set to perfectly size the elements array of the power set.
    int set_size = set->n_elements, pset_size = 1 << set_size;
    CSet* power_set = cset_create(sizeof(CSet*), pset_size, cset_compare, cset_cleanup, cset_genericToString);
    
    //Adds the empty set to the power set. Uses 1 as lowest capacity_hint since 0 triggers a default.
    CSet* empty = cset_create(set->elemsz, 1, set->cmp_fn, set->cleanup_fn, set->toString_fn);
    cset_add(power_set, &empty);

    //Uses a bit vector to exhaust every possible combination of elements to generate all subsets of the set.
    for(unsigned int bit_vector = 1; bit_vector < pset_size; bit_vector++) {
        //__builtin_popcount gives the number of active bits in an unsigned int.
        CSet* subset = cset_create(set->elemsz, __builtin_popcount(bit_vector), set->cmp_fn, set->cleanup_fn, set->toString_fn);
        for(int i = 0; i < set_size; i++) {
            if((bit_vector >> i) & 1) cset_add(subset, nth(set, i));
        }
        cset_add(power_set, &subset);
    }

    return power_set;
}

/* Function: cset_first
 * --------------------
 * Iterator. Returns the first element in the set (or NULL if empty).
 */ 
void* cset_first(CSet* set) {
    if(cset_isEmpty(set)) return NULL;
    return set->elements;
}

/* Function: cset_next
 * -------------------
 * Iterator. Given the previous element in the set, returns the next element. Returns NULL
 * to indicate that the end of the set has been reached.
 */ 
void* cset_next(CSet* set, void* prev) {
    int index = get_index(set, prev);
    if(index == set->n_elements - 1) return NULL;
    return nth(set, index + 1);
}

/* Function: cset_toString
 * -----------------------
 * Returns a heap-allocated string representation of the set. The client must provide a toString method
 * for the elements in their set in order for this function to work. If the client supplies NULL, then
 * a call to cset_toString will simply return NULL.
 */ 
char* cset_toString(CSet* set) {
    if(set->toString_fn == NULL) return NULL;
    char set_str[SET_STR_MAX_LEN];
    set_str[0] = '{'; set_str[1] = '\0';
    for(int i = 0; i < set->n_elements; i++) {
        char* elem_str = set->toString_fn(nth(set, i));
        strcat(set_str, elem_str);
        if(i != set->n_elements - 1) strcat(set_str, ", ");
        free(elem_str);
    }
    strcat(set_str, "}");
    return strdup(set_str);
}

/* Function: cset_compare
 * ----------------------
 * In order for sets to store other elements as sets, a void* compare function is needed, so one is
 * provided here. This function compares first by the number of elements in the set, then by the
 * size of the elements, and finally by using set1's compare function on each element.
 */ 
int cset_compare(const void* addr1, const void* addr2) {
    CSet* set1 = *(CSet **)addr1;
    CSet* set2 = *(CSet **)addr2;
    //Optimization: if the two sets are at the same address, they must be equal.
    if(set1 == set2) return 0;
    //First criterion for comparison is size; if sizes not equal, set with greater size is considered greater.
    if(set1->n_elements != set2->n_elements) return set1->n_elements - set2->n_elements;
    //Next criterion for comparison is elemsz; if different, set with greater elemsz is considered greater.
    if(set1->elemsz != set2->elemsz) return set1->elemsz - set2->elemsz;
    //If we get to this point, compare the elements of each set using set1's comparison function.
    for(int i = 0; i < set1->n_elements; i++) {
        int cmp_result = set1->cmp_fn(nth(set1, i), nth(set2, i));
        if(cmp_result != 0) return cmp_result;
    }
    return 0;
}

/* Function: cset_cleanup
 * ----------------------
 * Provided for use when nesting sets within other data structures (or sets) that require a cleanup function.
 */ 
void cset_cleanup(void* addr) {
    CSet* set = *(CSet **)addr;
    cset_delete(set);
}

/* Function: cset_genericToString
 * ------------------------------
 * Provided for use when creating a set of sets. Deferences the CSet** and calls the CSet toString function.
 */ 
char* cset_genericToString(const void* addr) {
    CSet* set = *(CSet **)addr;
    return cset_toString(set);
}

