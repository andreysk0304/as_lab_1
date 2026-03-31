#include "structures.h"

#include <stdlib.h>
#include <string.h>

Node* make_number(long long number) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    node->kind = NODE_NUMBER;
    node->number = number;
    node->var[0] = '\0';
    node->op = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* make_variable(const char* name) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    node->kind = NODE_VARIABLE;
    node->number = 0;
    strncpy(node->var, name, sizeof(node->var) - 1);
    node->var[sizeof(node->var) - 1] = '\0';
    node->op = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* make_operator(char op, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    node->kind = NODE_OPERATOR;
    node->number = 0;
    node->var[0] = '\0';
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}
