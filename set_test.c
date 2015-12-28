/* File: set_test.c
 * ----------------
 * Simple test program for the CSet data structure.
 */

#include "cset.h"
#include <stdio.h>
#include <string.h>

#define INT_STR_MAX 15

/* Simple comparator function for ints. */
int compare_ints(const void* addr1, const void* addr2) {
    int num1 = *(int *)addr1;
    int num2 = *(int *)addr2;
    return num1 - num2;
}

/* Simple toString function for ints. */
char* print_int(const void* addr) {
    int num = *(int *)addr;
    char str[INT_STR_MAX];
    sprintf(str, "%d", num);
    return strdup(str);
}

/* Simple comparator function for strings. */
int compare_strs(const void* addr1, const void* addr2) {
    char* str1 = *(char **)addr1;
    char* str2 = *(char **)addr2;
    return strcmp(str1, str2);
}

/* Simple cleanup function for strings. */
void cleanup_str(void* addr) {
    char* str = *(char **)addr;
    free(str);
}

/* Simple toString function for strings. */
char* print_str(const void* addr) {
    char* str = *(char **)addr;
    return strdup(str);
}

/* Helper function that prints the elements of a set. */
void print_set(CSet* set) {
    char* set_string = cset_toString(set);
    printf("%s\n", set_string);
    free(set_string);
}

/* Recursive helper function that creates a nested set of order n */
CSet* create_nested_set(int n) {
    if(n == 0) return cset_create(sizeof(CSet*), 1, cset_compare, cset_cleanup, cset_genericToString);

    CSet* set = cset_create(sizeof(CSet*), 1, cset_compare, cset_cleanup, cset_genericToString);
    CSet* elem_set = create_nested_set(n - 1);
    cset_add(set, &elem_set);
    return set;
}

/* Test using a set of ints. */
void simple_test() {
    printf("\nCreating set...\n");
    CSet* set = cset_create(sizeof(int), 10, compare_ints, NULL, print_int);

    print_set(set);

    printf("\nAdding a few elements...\n");
    int values[] = {0, 2, 5, 9, 13, 1, 7, 42};
    for(int i = 0; i < 8; i++) {
        cset_add(set, &values[i]);
        print_set(set);
    }

    printf("\nSet isEmpty? (expect false): %s\n", cset_isEmpty(set) ? "true" : "false");
    printf("Set has %d elements. (expect 8)\n", cset_size(set));
    
    print_set(set);

    printf("\nRemoving elements...\n");
    for(int i = 7; i >= 0; i--) {
        cset_remove(set, &values[i]);
        print_set(set);
    }

    printf("\nAdding elements again and removing with clear...\n");
    for(int i = 0; i < 8; i++) {
        cset_add(set, &values[i]);
    }
    print_set(set);
    cset_clear(set);
    print_set(set);

    printf("\nDeleting set...\n");
    cset_delete(set);
    printf("Done!\n\n");
}

/* Test for nesting sets. */
void nested_sets_test() {
    printf("\nCreating several nested sets...\n");
    CSet* set = cset_create(sizeof(CSet*), 1, cset_compare, cset_cleanup, cset_genericToString);
    
    for(int i = 0; i < 5; i++) {
        CSet* elem_set = create_nested_set(i);
        cset_add(set, &elem_set);
        print_set(set);
    }

    printf("\nCreating a set containing a set of ints and a set of strings...\n");
    CSet* diff_types = cset_create(sizeof(CSet*), 2, cset_compare, cset_cleanup, cset_genericToString);

    int ints[] = {137, 1, 42};
    char* strs[] = {"hello", "goodbye", "power set"};

    CSet* int_set = cset_create(sizeof(int), 3, compare_ints, NULL, print_int);
    //Cleanup function is NULL because elements are string constants.
    CSet* str_set = cset_create(sizeof(char*), 3, compare_strs, NULL, print_str);

    for(int i = 0; i < 3; i++) {
        cset_add(int_set, &ints[i]);
        cset_add(str_set, &strs[i]);
    }
    
    printf("\nA set of ints: "); print_set(int_set);
    printf("A set of strings: "); print_set(str_set);

    printf("\nAdding both sets to our set contianing sets...\n");
    cset_add(diff_types, &str_set);
    cset_add(diff_types, &int_set);
    printf("Final state of our set after additions: "); print_set(diff_types);

    printf("\nDeleting nested sets...\n");
    cset_delete(set);
    cset_delete(diff_types);

    printf("Done!\n\n");
}

/* Test of set operations. */
void set_ops_test() {
    printf("\nCreating a few sets to test set operations with...\n");
    int vals1[] = {1, 3, 5, 6, 9, 12, 15};
    int vals2[] = {1, 2, 3, 4, 5, 6, 9, 12, 13, 15, 18, 19};

    CSet* set1 = cset_create(sizeof(int), 7, compare_ints, NULL, print_int);
    for(int i = 0; i < 7; i++) {
        cset_add(set1, &vals1[i]);
    }

    CSet* set2 = cset_create(sizeof(int), 12, compare_ints, NULL, print_int);
    for(int i = 0; i < 12; i++) {
        cset_add(set2, &vals2[i]);
    }

    printf("\nSet1: "); print_set(set1);
    printf("Set2: "); print_set(set2);
    printf("\nIs set1 is a subset of set2? (expect true): %s\n", cset_isSubsetOf(set1, set2) ? "true" : "false");
    printf("Is set2 is a subset of set1? (expect false): %s\n", cset_isSubsetOf(set2, set1) ? "true" : "false");

    int vals3[] = {1, 2, 3, 4, 5};
    int vals4[] = {8, 7, 6, 5, 4};

    CSet* set3 = cset_create(sizeof(int), 5, compare_ints, NULL, print_int);
    CSet* set4 = cset_create(sizeof(int), 5, compare_ints, NULL, print_int);
    for(int i = 0; i < 5; i++) {
        cset_add(set3, &vals3[i]);
        cset_add(set4, &vals4[i]);
    }

    printf("\nSet3: "); print_set(set3);
    printf("Set4: "); print_set(set4);

    CSet* u = cset_union(set3, set4);
    printf("\nUnion of Set3 and Set4: "); print_set(u);

    CSet* intersect = cset_intersect(set3, set4);
    printf("\nIntersect of Set3 and Set4: "); print_set(intersect);

    CSet* diff34 = cset_difference(set3, set4);
    CSet* diff43 = cset_difference(set4, set3);
    CSet* symm_diff = cset_symmetricDifference(set3, set4);
    printf("\nSet3 - Set4: "); print_set(diff34);
    printf("Set4 - Set3: "); print_set(diff43);
    printf("Symmetric Difference of Set3 and Set4: "); print_set(symm_diff);

    CSet* power_set1 = cset_powerSet(set1);
    CSet* power_set3 = cset_powerSet(set3);
    printf("\nPower set of Set1: "); print_set(power_set1);
    printf("\nPower set of Set3: "); print_set(power_set3);

    printf("\nDeleting original sets and results of operations...\n");
    cset_delete(set1);
    cset_delete(set2);
    cset_delete(set3);
    cset_delete(set4);
    cset_delete(u);
    cset_delete(intersect);
    cset_delete(diff34);
    cset_delete(diff43);
    cset_delete(symm_diff);
    cset_delete(power_set1);
    cset_delete(power_set3);
    printf("Done!\n\n");
}

int main(int argc, char* argv[]) {
    simple_test();
    nested_sets_test();
    set_ops_test();
    return 0;
}
