#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "table.h"

Table make_sample_table(void) {
    Table table;
    initTable(&table);

    addRow(&table, "3", "привет");
    addRow(&table, "1", "пока");
    addRow(&table, "2", "как дела ?");
    addRow(&table, "10", "йохохо");
    addRow(&table, "5", "на абордаж");

    return table;
}

void test_init_table(void) {
    printf("test 1, init table starting...\n");
    Table table;
    initTable(&table);

    assert(table.size == 0);
    assert(table.values == NULL);
    assert(table.keys == NULL);
    printf("test 1, init table complete!\n");
}

void test_addRow(void) {
    printf("test 2, add row starting...\n");
    Table t;
    initTable(&t);

    addRow(&t, "1", "hello");
    assert(t.size == 1);
    assert(t.keys != NULL);
    assert(t.values != NULL);
    assert(strcmp(t.keys[0], "1") == 0);
    assert(strcmp(t.values[0], "hello") == 0);

    freeTable(&t);
    printf("test 2, add row complete!\n");
}

void test_swapRows(void) {
    printf("test 3, swap rows starting...\n");
    Table t;
    initTable(&t);

    addRow(&t, "1", "aaa");
    addRow(&t, "2", "bbb");

    swapRows(&t, 0, 1);

    assert(strcmp(t.keys[0], "2") == 0);
    assert(strcmp(t.values[0], "bbb") == 0);
    assert(strcmp(t.keys[1], "1") == 0);
    assert(strcmp(t.values[1], "aaa") == 0);

    freeTable(&t);
    printf("test 3, swap rows complete\n");
}

void test_copyTable(void) {
    printf("test 4, copy table starting...\n");
    Table a = make_sample_table();
    Table b = copyTable(&a);

    assert(b.size == a.size);
    for (int i = 0; i < a.size; i++) {
        assert(strcmp(a.keys[i], b.keys[i]) == 0);
        assert(strcmp(a.values[i], b.values[i]) == 0);
    }

    free(a.keys[0]);
    a.keys[0] = malloc(2);
    strcpy(a.keys[0], "X");
    assert(strcmp(b.keys[0], "3") == 0);

    freeTable(&a);
    freeTable(&b);
    printf("test 4, copy table complete\n");
}

void test_insertionSort(void) {
    printf("test 5, insertion sort starting...\n");
    Table t = make_sample_table();

    insertionSort(&t);

    assert(strcmp(t.keys[0], "1") == 0);
    assert(strcmp(t.keys[1], "10") == 0);
    assert(strcmp(t.keys[2], "2") == 0);
    assert(strcmp(t.keys[3], "3") == 0);
    assert(strcmp(t.keys[4], "5") == 0);

    assert(strcmp(t.values[0], "пока") == 0);
    assert(strcmp(t.values[1], "йохохо") == 0);
    assert(strcmp(t.values[2], "как дела ?") == 0);
    assert(strcmp(t.values[3], "привет") == 0);
    assert(strcmp(t.values[4], "на абордаж") == 0);

    freeTable(&t);
    printf("test 5, insertion sort complete!\n");
}

void test_binarySearch_found_and_not_found(void) {
    printf("test 6, binary search found and not found starting...\n");
    Table t = make_sample_table();
    insertionSort(&t);

    int i1 = binarySearch(&t, "1");
    assert(i1 >= 0);
    assert(strcmp(t.keys[i1], "1") == 0);
    assert(strcmp(t.values[i1], "пока") == 0);

    int i10 = binarySearch(&t, "10");
    assert(i10 >= 0);
    assert(strcmp(t.keys[i10], "10") == 0);
    assert(strcmp(t.values[i10], "йохохо") == 0);

    int imiss = binarySearch(&t, "999");
    assert(imiss == -1);

    freeTable(&t);
    printf("test 6, binary search found and not found complete!\n");
}

void test_reverseTable(void) {
    printf("test 7, reverse table starting\n");
    Table t;
    initTable(&t);

    addRow(&t, "1", "a");
    addRow(&t, "2", "b");
    addRow(&t, "3", "c");

    reverseTable(&t);

    assert(strcmp(t.keys[0], "3") == 0);
    assert(strcmp(t.keys[1], "2") == 0);
    assert(strcmp(t.keys[2], "1") == 0);

    assert(strcmp(t.values[0], "c") == 0);
    assert(strcmp(t.values[1], "b") == 0);
    assert(strcmp(t.values[2], "a") == 0);

    freeTable(&t);
    printf("test 7, reverse table complete\n");
}

void test_shuffleTable_basic(void) {
    printf("test 8, shuffle table basic starting...\n");
    srand(123);

    Table t = make_sample_table();

    int count_1 = 0, count_2 = 0, count_3 = 0, count_5 = 0, count_10 = 0;
    for (int i = 0; i < t.size; i++) {
        if (strcmp(t.keys[i], "1") == 0) count_1++;
        if (strcmp(t.keys[i], "2") == 0) count_2++;
        if (strcmp(t.keys[i], "3") == 0) count_3++;
        if (strcmp(t.keys[i], "5") == 0) count_5++;
        if (strcmp(t.keys[i], "10") == 0) count_10++;
    }
    assert(count_1 == 1 && count_2 == 1 && count_3 == 1 && count_5 == 1 && count_10 == 1);

    shuffleTable(&t);

    count_1 = count_2 = count_3 = count_5 = count_10 = 0;
    for (int i = 0; i < t.size; i++) {
        if (strcmp(t.keys[i], "1") == 0) count_1++;
        if (strcmp(t.keys[i], "2") == 0) count_2++;
        if (strcmp(t.keys[i], "3") == 0) count_3++;
        if (strcmp(t.keys[i], "5") == 0) count_5++;
        if (strcmp(t.keys[i], "10") == 0) count_10++;
    }
    assert(count_1 == 1 && count_2 == 1 && count_3 == 1 && count_5 == 1 && count_10 == 1);

    freeTable(&t);
    printf("test 8, shuffle table basic complete\n");
}

void test_freeTable_resets(void) {
    printf("test 9, free table resets startig...\n");
    Table t = make_sample_table();
    freeTable(&t);

    assert(t.size == 0);
    assert(t.keys == NULL);
    assert(t.values == NULL);
    printf("test 9, free table resets complete!\n");
}

int main(void) {
    printf("STARTING TESTS ...\n");

    test_init_table();
    test_addRow();
    test_swapRows();
    test_copyTable();
    test_insertionSort();
    test_binarySearch_found_and_not_found();
    test_reverseTable();
    test_shuffleTable_basic();
    test_freeTable_resets();

    printf("ALL TESTS PASSED\n");
    return 0;
}