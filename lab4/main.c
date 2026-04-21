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
    double number = 0.0;

    printf("Ключ: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }

    printf("Значение: ");
    if (scanf("%lf", &number) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    if (insert_tree(tree, key, number)) {
        printf("Узел добавлен.\n");
    }
}

void deleteNodeMenu(BTree* tree) {
    char key[64];

    printf("Ключ для удаления: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    if (delete_tree(tree, key)) {
        printf("Узел удалён.\n");
    }
}

void searchNodeMenu(BTree* tree) {
    char key[64];
    double number = 0.0;

    printf("Ключ для поиска: ");
    if (scanf("%63s", key) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    if (is_valid_key(key) && search_tree(tree->root, key, &number)) {
        printf("Найдено: \"%s\" -> %.10g\n", key, number);
    } else if (!is_valid_key(key)) {
        printf("Некорректный ключ.\n");
    } else {
        printf("Ключ не найден.\n");
    }
}

void processFileMenu(BTree* tree) {
    char in_name[256];
    char out_name[256];

    printf("Имя входного файла: ");
    if (scanf("%255s", in_name) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }

    printf("Имя выходного файла: ");
    if (scanf("%255s", out_name) != 1) {
        printf("Ошибка ввода.\n");
        clearBuffer();
        return;
    }
    clearBuffer();

    clear_tree(tree);
    if (process_commands_from_file(tree, in_name, out_name)) {
        printf("Команды выполнены. Результат записан в %s\n", out_name);
    } else {
        printf("Не удалось обработать файл.\n");
    }
}

int main(void) {
    BTree tree;
    int cmd = 0;

    init_tree(&tree);

    while (1) {
        showMenu();

        if (scanf("%d", &cmd) != 1) {
            printf("Введите число.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (cmd) {
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
