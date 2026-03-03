#include "table.h"

int main(void) {
    Table table;
    initTable(&table);

    readTableFromFile(&table, "input.txt");

    printTable(&table);

    insertionSort(&table);

    writeTableToFile(&table, "output.txt");

    freeTable(&table);

    return 0;
}