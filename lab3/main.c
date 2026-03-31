#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "tree.h"

#define MAX_INPUT_LEN 1024

char* read_input_line(void) {
    size_t capacity = 16;
    size_t length = 0;
    char* buffer = (char*)malloc(capacity);
    int c = 0;

    if (!buffer) return NULL;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (length >= MAX_INPUT_LEN) continue;

        if (length + 1 >= capacity) {
            size_t new_capacity = capacity * 2;
            char* new_buffer = NULL;

            if (new_capacity > MAX_INPUT_LEN + 1) {
                new_capacity = MAX_INPUT_LEN + 1;
            }

            new_buffer = (char*)realloc(buffer, new_capacity);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }

            buffer = new_buffer;
            capacity = new_capacity;
        }

        buffer[length] = (char)c;
        length++;
    }

    if (c == EOF && length == 0) {
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

int is_empty_line(const char* text) {
    for (const char* current = text; *current; current++) {
        if (!isspace(*current)) return 0;
    }
    return 1;
}

void print_parse_error(int error) {
    switch (error) {
        case PARSE_NULL_INPUT:
            printf("Ошибка: пустой вход.\n");
            break;
        case PARSE_EMPTY_INPUT:
            printf("Ошибка: выражение пустое.\n");
            break;
        case PARSE_MEMORY_ERROR:
            printf("Ошибка: не удалось выделить память.\n");
            break;
        case PARSE_EXPECTED_OPERAND:
            printf("Ошибка: ожидалось число, переменная или выражение в скобках.\n");
            break;
        case PARSE_MISSING_RPAREN:
            printf("Ошибка: пропущена закрывающая скобка.\n");
            break;
        case PARSE_INVALID_TOKEN:
            printf("Ошибка: недопустимый символ во входном выражении.\n");
            break;
        case PARSE_TRAILING_DATA:
            printf("Ошибка: после корректного выражения остались лишние символы.\n");
            break;
        default:
            printf("Ошибка разбора.\n");
            break;
    }
}

int main(void) {
    while (1) {
        int error = 0;
        char* input = NULL;
        char* rpn = NULL;
        Node* root = NULL;

        printf("\nВведите выражение (пустая строка для выхода):\n> ");
        input = read_input_line();
        if (!input) break;

        if (is_empty_line(input)) {
            free(input);
            break;
        }

        printf("\nИсходное выражение: %s\n", input);

        rpn = convert_to_rpn(input, &error);
        if (!rpn) {
            free(input);
            print_parse_error(error);
            continue;
        }

        printf("ОПН: %s\n", rpn);

        root = build_tree_from_rpn(rpn, &error);
        if (!root) {
            free(input);
            free(rpn);
            print_parse_error(error);
            continue;
        }

        printf("\nДерево до упрощения:\n");
        print_tree(root, 0);

        simplify_mul_by_zero(root);

        printf("\nРезультат:\n");
        print_expression(root, stdout);
        printf("\n");

        printf("\nДерево после упрощения:\n");
        print_tree(root, 0);

        free(input);
        free_tree(root);
        free(rpn);
    }

    printf("Работа завершена.\n");
    return 0;
}
