#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tree.h"

int getTreeDepth(Node* root) {
    if (root == NULL) return 0;

    int leftDepth = getTreeDepth(root->left);
    int rightDepth = getTreeDepth(root->right);

    return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
}

bool isChild(Node* node) {
    if (!node) return false;
    return node -> left == NULL && node -> right == NULL;
}

void nodeCreateTest() {
    printf("test 1, create node test\n");
    Node* node = createNode(67);

    printf("test 1, creating node...\n");
    assert(node != NULL);
    assert(node -> left == NULL);
    assert(node -> right == NULL);
    assert(node -> data == 67);

    printf("test 1, creating node complete\n");

    free(node);
    printf("test 1, status: OK\n\n");
}

void insertNodeTest() {
    printf("test 2, insert node test\n");
    Node* root = NULL;

    printf("test 1, inserting nodes...\n");
    insert(&root, 2);
    assert(getTreeDepth(root) == 1);

    insert(&root, 3);
    assert(getTreeDepth(root) == 2);

    insert(&root, 4);
    assert(getTreeDepth(root) == 3);

    insert(&root, 1);
    assert(getTreeDepth(root) == 3);

    assert(root != NULL);

    assert(isChild(root -> left));
    assert(isChild(root -> right -> right));
    printf("test 2, inserting node complete\n");

    freeTree(root);
    printf("test 2, status: OK\n\n");
}

void deleteChildTest() {
    printf("test 3, delete child test\n");
    Node* root = NULL;


    printf("test 3, inserting data...\n");
    insert(&root, 50);
    insert(&root, 30);
    insert(&root, 70);
    insert(&root, 20);
    insert(&root, 40);

    assert(getTreeDepth(root) != 0);
    printf("test 3, inserting data complete\n");

    printf("test 3, deleting data...\n");
    deleteNode(&root, 20);
    assert(root->left->left == NULL);

    deleteNode(&root, 40);
    assert(root->left->right == NULL);
    assert(isChild(root->left));
    printf("test 3, deleting data complete\n");

    freeTree(root);
    printf("test 3, status: OK\n\n");
}

void isSymmetricStructTest() {
    printf("test 4, is symmetric struct test\n");

    Node* root = NULL;
    assert(isSymmetricStruct(root) == true);

    root = createNode(10);
    assert(isSymmetricStruct(root) == true);

    printf("test 4, creating symmetric struct...\n");
    root->left = createNode(5);
    root->right = createNode(7);
    root->left->left = createNode(2);
    root->left->right = createNode(3);
    root->right->left = createNode(4);
    root->right->right = createNode(5);
    printf("test 4, creating symmetric struct complete\n");

    assert(isSymmetricStruct(root) == true);

    free(root->right->right);
    root->right->right = NULL;

    assert(isSymmetricStruct(root) == false);

    freeTree(root);
    printf("test 4, status: OK\n\n");
}

int main(void) {
    printf("ТЕСТЫ ДЛЯ БИНАРНОГО ДЕРЕВА И СИММЕТРИИ СТРУКТУРЫ\n\n");

    nodeCreateTest();
    insertNodeTest();
    deleteChildTest();
    isSymmetricStructTest();


    printf("ТЕСТЫ ОКОНЧЕНЫ");
}