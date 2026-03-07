#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "table.h"

#define MIN_TABLE_SIZE 13
#define MAX_INPUT_LEN 4096

char* read_line_stdin(void) {
    size_t size = 16;
    size_t len = 0;

    char* buf = malloc(size);
    if (buf == NULL)
        return NULL;

    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (len >= MAX_INPUT_LEN) {
            continue;
        }
        if (len + 1 >= size) {
            size_t new_size = size * 2;
            if (new_size > MAX_INPUT_LEN + 1) new_size = MAX_INPUT_LEN + 1;

            char* tmp = realloc(buf, new_size);
            if (tmp == NULL) {
                free(buf);
                return NULL;
            }

            buf = tmp;
            size = new_size;
        }

        buf[len] = (char)c;
        len++;
    }

    char* tmp = realloc(buf, len + 1);
    if (tmp != NULL) buf = tmp;

    buf[len] = '\0';

    return buf;
}

char* read_prompt(const char* prompt) {
    printf("%s", prompt);
    return read_line_stdin();
}

void generate_input_file(const char* filename, int count) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("Не удалось создать файл %s\n", filename);
        return;
    }
    for (int i = 0; i < count; i++) fprintf(f, "%d %d\n", rand(), rand());
    fclose(f);
}

void showMenu(void) {
    printf("\n\n1. Прочитать таблицу из файла (input.txt)\n");
    printf("2. Добавить запись\n");
    printf("3. Удалить запись по ключу\n");
    printf("4. Вывести таблицу в консоль\n");
    printf("5. Записать таблицу в файл\n");
    printf("6. Найти значение по ключу (бинарный поиск)\n");
    printf("7. Проверка сортировки (3 случая)\n");
    printf("8. Сгенерировать input.txt (случайные целые)\n");
    printf("9. Очистить таблицу\n");
    printf("10. Отсортировать таблицу по ключам\n");
    printf("11. Перевернуть таблицу\n");
    printf("12. Перемешать таблицу\n");
    printf("0. Завершить работу\n");
    printf("> ");
}

void runSortTestCases(Table* table) {
    if (table->size == 0) {
        printf("Таблица пуста. Загрузите данные из файла или добавьте записи\n");
        return;
    }

    if (table->size < MIN_TABLE_SIZE) printf("Рекомендуется не менее %d элементов\n", MIN_TABLE_SIZE);

    Table t;

    printf("\n=== Случай 1: элементы упорядочены ===\n");
    t = copyTable(table);
    insertionSort(&t);
    printf("Исходная таблица:\n");
    printTable(&t);
    printf("Вызов сортировки...\n");
    insertionSort(&t);
    printf("После сортировки:\n");
    printTable(&t);
    freeTable(&t);

    printf("\n=== Случай 2: обратный порядок ===\n");
    t = copyTable(table);
    insertionSort(&t);
    reverseTable(&t);
    printf("Исходная таблица:\n");
    printTable(&t);
    printf("Вызов сортировки...\n");
    insertionSort(&t);
    printf("После сортировки:\n");
    printTable(&t);
    freeTable(&t);

    printf("\n=== Случай 3: не упорядочены ===\n");
    t = copyTable(table);
    srand(time(NULL));
    shuffleTable(&t);
    printf("Исходная таблица:\n");
    printTable(&t);
    printf("Вызов сортировки...\n");
    insertionSort(&t);
    printf("После сортировки:\n");
    printTable(&t);
    freeTable(&t);

    printf("\nПроверка трёх случаев завершена.\n");
}

void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void addRowMenu(Table* table) {
    char* key = read_prompt("Ключ: ");

    if (!key) {
        printf("Ошибка ввода.\n");
        return;
    }

    char* value = read_prompt("Значение: ");

    if (!value) {
        free(key); printf("Ошибка ввода.\n");
        return;
    }

    addRow(table, key, value);
    free(key);
    free(value);

    printf("Запись добавлена.\n");
}

void removeRowMenu(Table* table) {
    char* key = read_prompt("Ключ для удаления: ");
    if (!key) {
        printf("Ошибка ввода.\n");
        return;
    }
    removeRowByKey(table, key);
    free(key);
}

void writeFileMenu(Table* table) {
    char* filename = read_prompt("Имя файла: ");
    if (!filename) {
        printf("Ошибка ввода.\n");
        return;
    }
    writeTableToFile(table, filename);
    printf("Таблица записана в %s\n", filename);
    free(filename);
}

void searchMenu(Table* table) {
    char* key = read_prompt("Ключ для поиска: ");
    if (!key) {
        printf("Ошибка ввода.\n");
        return;
    }
    insertionSort(table);
    int idx = binarySearch(table, key);

    if (idx >= 0)
        printf("Найдено: ключ \"%s\" -> значение \"%s\"\n", table->keys[idx], table->values[idx]);
    else
        printf("Запись с ключом \"%s\" не найдена.\n", key);
    free(key);
}

int main(void) {
    Table table;
    initTable(&table);
    int input;

    while (1) {
        showMenu();

        if (scanf("%d", &input) != 1) {
            printf("Введите число.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (input) {
            case 0:
                freeTable(&table);
                printf("Память освобождена. До следующей лабы! ;)\n");
                return 0;
            case 1:
                readTableFromFile(&table, "input.txt");
                printf("Таблица загружена из input.txt\n");
                break;
            case 2:
                addRowMenu(&table);
                break;
            case 3:
                removeRowMenu(&table);
                break;
            case 4:
                printTable(&table);
                break;
            case 5:
                writeFileMenu(&table);
                break;
            case 6:
                searchMenu(&table);
                break;
            case 7:
                runSortTestCases(&table);
                break;
            case 8: {
                int n;
                printf("Количество строк (целое): ");
                if (scanf("%d", &n) == 1 && n > 0) {
                    clearBuffer();
                    srand(time(NULL));
                    generate_input_file("input.txt", n);
                    printf("Сгенерирован input.txt: %d строк\n", n);
                } else {
                    clearBuffer();
                    printf("Ошибка ввода.\n");
                }
                break;
            }
            case 9: {
                freeTable(&table);
                printf("Таблица успешно очищена!\n");
                break;
            }
            case 10: {
                printf("Таблица до сортировки:\n");
                printTable(&table);
                printf("\n------------\n\n");
                insertionSort(&table);
                printTable(&table);
                break;
            }
            case 11: {
                reverseTable(&table);
                printf("Таблица успешно перевёрнута!\n");
                break;
            }
            case 12: {
                shuffleTable(&table);
                printf("Таблица успешно перемешана!\n");
                break;
            }
            default:
                printf("Неизвестная команда!\n");
                break;
        }
    }

    return 0;
}
