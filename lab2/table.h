#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>

typedef struct {
    char** keys;
    char** values;
    int size;
} Table;

void initTable(Table* table);
void addRow(Table* table, char* key, char* value);
void freeTable(Table* table);
void printTable(Table* table);
void readTableFromFile(Table* table, const char* filename);

void swapRows(Table* table, int i, int j);
Table copyTable(Table* src);

void insertionSort(Table* table);
int binarySearch(Table* table, const char* key);

void reverseTable(Table* table);
void shuffleTable(Table* table);

#endif