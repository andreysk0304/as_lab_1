#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

void showMenu(void) {
    printf("\n1. Добавить узел\n");
    printf("2. Удалить узел\n");
    printf("3. Вывести дерево\n");
    printf("4. Найти значение по ключу\n");
    printf("5. Выполнить команды из файла\n");
    printf("0. Завершить работу\n");
    printf("> ");
}

void clearBuffer(void) {
    int c = 0;

    while ((c = getchar()) != '\n' && c != EOF);
}

void addNodeMenu(BTree* tree) {
    char key[64];
    double value = 0.0;
    BTreeStatus status = BTREE_OK;

    printf("Ключ: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }

    printf("Значение: ");
    if (scanf("%lf", &value) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    status = insert_tree(tree, key, value);
    if (status == BTREE_OK) {
        printf("Узел добавлен.\n");
    } else if (status == BTREE_DUPLICATE_KEY) {
        printf("Такой ключ уже есть в дереве.\n");
    } else if (status == BTREE_INVALID_KEY) {
        printf("Ключ должен содержать только латинские буквы и быть длиной от 1 до 6 символов.\n");
    } else {
        printf("Не удалось добавить узел.\n");
    }
}

void deleteNodeMenu(BTree* tree) {
    char key[64];
    BTreeStatus status = BTREE_OK;

    printf("Ключ для удаления: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    status = delete_tree(tree, key);
    if (status == BTREE_OK) {
        printf("Узел удалён.\n");
    } else if (status == BTREE_KEY_NOT_FOUND) {
        printf("Ключ не найден.\n");
    } else if (status == BTREE_INVALID_KEY) {
        printf("Некорректный ключ.\n");
    } else {
        printf("Не удалось удалить узел.\n");
    }
}

void searchNodeMenu(BTree* tree) {
    char key[64];
    double value = 0.0;
    BTreeStatus status = BTREE_OK;

    printf("Ключ для поиска: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    status = search_tree(tree->root, key, &value);
    if (status == BTREE_OK) {
        printf("Найдено: \"%s\" -> %.10g\n", key, value);
    } else if (status == BTREE_KEY_NOT_FOUND) {
        printf("Ключ не найден.\n");
    } else {
        printf("Некорректный ключ.\n");
    }
}

void processFileMenu(BTree* tree) {
    char input_filename[256];
    char output_filename[256];
    BTreeStatus status = BTREE_OK;

    printf("Имя входного файла: ");
    if (scanf("%255s", input_filename) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }

    printf("Имя выходного файла: ");
    if (scanf("%255s", output_filename) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    clear_tree(tree);
    status = process_commands_from_file(tree, input_filename, output_filename);

    if (status == BTREE_OK) {
        printf("Команды выполнены. Результат записан в %s\n", output_filename);
    } else {
        printf("Не удалось обработать файл.\n");
    }
}

int main(void) {
    BTree tree;
    int input = 0;

    init_tree(&tree);

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
                clear_tree(&tree);
                printf("Память освобождена. До следующей лабы!\n");
                return 0;
            case 1:
                addNodeMenu(&tree);
                break;
            case 2:
                deleteNodeMenu(&tree);
                break;
            case 3:
                print_tree(tree.root, 0, stdout);
                break;
            case 4:
                searchNodeMenu(&tree);
                break;
            case 5:
                processFileMenu(&tree);
                break;
            default:
                printf("Команда не найдена.\n");
                break;
        }
    }

    return 0;
}
