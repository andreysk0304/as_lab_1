#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char** keys;
    char** values;
    int size;
} Table;


void initTable(Table* table) {
    table->size = 0;
    table->keys = NULL;
    table->values = NULL;
}


void addRow(Table* table, char* key, char* value) {
    table -> size ++;

    table -> keys = realloc(table->keys, table->size * sizeof(char*));
    table -> values = realloc(table->values, table->size * sizeof(char*));

    table -> keys[table -> size - 1] = malloc(strlen(key) + 1);
    table -> values[table -> size - 1] = malloc(strlen(value) + 1);

    strcpy(table -> keys[table -> size - 1], key);
    strcpy(table -> values[table -> size - 1], value);
}


void freeTable(Table* table) {
    int i = 0;
    for (int i = 0; i < table->size; i++) {
        free(table->keys[i]);
        free(table->values[i]);
    }
    free(table->keys);
    free(table->values);

    table->keys = NULL;
    table->values = NULL;
    table->size = 0;
}


void printTable(Table* table) {
    if (!table) {
        printf("Таблица пуста!");
        return;
    }

    printf("\nТаблица\n");

    int i = 0;
    for (i = 0; i<table -> size; i++) {
        printf("%d  %s  %s\n", i + 1, table->keys[i], table->values[i]);
    }
}


void readTableFromFile(Table* table, const char* filename) {
    FILE* file = fopen(filename, "r");

    if (!file) {
        printf("\nВходная таблица не найдена!\n");
        return;
    }

    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {

        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) continue;

        char* key = strtok(line, " ");
        char* value = strtok(NULL, "");

        if (!key) continue;
        if (!value) value = "";

        addRow(table, key, value);
    }

    free(line);
    fclose(file);
}


void swapRows(Table* table, int i, int j) {
    char* tmp;

    tmp = table -> keys[i];
    table -> keys[i] = table -> keys[j];
    table -> keys[j] = tmp;

    tmp = table -> values[i];
    table -> values[i] = table -> values[j];
    table -> values[j] = tmp;
}


Table copyTable(Table* table) {
    Table new_table;
    initTable(&new_table);

    int i = 0;
    for (i = 0; i < table -> size; i++) {
        addRow(&new_table, table -> keys[i], table -> values[i]);
    }

    return new_table;
}


void insertionSort(Table* table) {
    for (int i = 1; i < table->size; i++) {
        char* tmpKey = table->keys[i];
        char* tmpValue = table->values[i];

        int j = i - 1;
        while (j >= 0 && strcmp(table->keys[j], tmpKey) > 0) {
            table->keys[j + 1] = table->keys[j];
            table->values[j + 1] = table->values[j];
            j--;
        }
        table->keys[j + 1] = tmpKey;
        table->values[j + 1] = tmpValue;
    }
}


void reverseTable(Table* table) {
    int i = 0;
    for (i = 0; i < table -> size / 2; i++) {
        swapRows(table, i, table -> size - 1 - i);
    }
}


void shuffleTable(Table* table) {
    int i = 0;
    for (i = table -> size - 1; i > 0; i-- ) {
        int j = rand() % (i+1);
        swapRows(table, i, j);
    }
}


int binarySearch(Table* table, const char* key) {
    int left = 0, right = table -> size - 1;

    while (left <= right) {
        int middle = (left + right) / 2;
        int cmp = strcmp(table -> keys[middle], key);

        if (cmp == 0) return middle;
        if (cmp < 0) left = middle + 1;
        else right = middle - 1;
    }
    return -1;
}


void writeTableToFile(Table* table, const char* filename) {
    if (!table) {
        printf("Таблица пуста!\n");
        return;
    }

    FILE* out = fopen(filename, "w");
    if (!out) {
        fprintf(out, "Не удалось открыть output файл");
        return;
    }

    fprintf(out, "Таблица\n");
    for (int i = 0; i < table->size; i++) {
        fprintf(out, "%s  %s\n", table->keys[i], table->values[i]);
    }

    fclose(out);
}