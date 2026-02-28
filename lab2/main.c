#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

int main(void) {
    Table table;
    initTable(&table);

    readTableFromFile(&table, "input.txt");

    printTable(&table);

    freeTable(&table);

    return 0;
}