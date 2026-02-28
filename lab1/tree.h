#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int data);
void insert(Node** root, int data);
void deleteNode(Node** root, int data);
void printTree(Node* root, int depth);
void freeTree(Node* root);
bool isMirrorStruct(Node* left, Node* right);
bool isSymmetricStruct(Node* root);
Node* findMin(Node* root);

#endif