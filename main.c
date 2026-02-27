#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tree.h"

void showMenu() {
    printf("1. Добавить ноду\n");
    printf("2. Вывести дерево\n");
    printf("3. Удалить ноду\n");
    printf("4. Проверить является ли дерево симметричным\n");
    printf("0. Завершить работу\n");
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void createNodeMenu(Node** root) {
    int input_node_menu;

    while (1) {
        printf("Enter your data: ");
        if (scanf("%d", &input_node_menu) == 1) {
            insert(root, input_node_menu);
            printf("New node created!\n");
            clearBuffer();
            break;
        }

        printf("You entered not integer input!\n");
        clearBuffer();
    }
}

void deleteNodeMenu(Node** root) {
    int input_node_menu;

    while (1) {
        printf("Enter your data: ");
        if (scanf("%d", &input_node_menu) == 1) {
            deleteNode(root, input_node_menu);
            printf("Node removed!\n");
            clearBuffer();
            break;
        }

        printf("You entered not integer input!\n");
        clearBuffer();
    }
}

int main(void) {
    Node* root = NULL;
    int input;

    while (1) {
        showMenu();

        if (scanf("%d", &input) == 1) {
            clearBuffer();

            switch (input) {
                case 0:
                    freeTree(root);
                    printf("Memory is clear.\nGoodluck!\n");
                    exit(0);
                case 1:
                    createNodeMenu(&root);
                    break;
                case 2:
                    printTree(root, 0);
                    break;
                case 3:
                    deleteNodeMenu(&root);
                    break;
                case 4:
                    if (isMirrorStruct(root, root) == true) {
                        printf("You entered mirred struct!\n");
                        break;
                    }
                    printf("You entered not mirred struct!\n");
                    break;
                default:
                    printf("Command not found!\n");
                    break;
            }
        } else {
            printf("You entered not integer input!\n");
            clearBuffer();
        }
    }

    return 0;
}