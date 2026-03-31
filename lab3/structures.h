#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef enum NodeKind {
    NODE_NUMBER,
    NODE_OPERATOR,
    NODE_VARIABLE
} NodeKind;

typedef struct Node {
    NodeKind kind;
    long long number;
    char var[32];
    char op;
    struct Node* left;
    struct Node* right;
} Node;

Node* make_number(long long number);
Node* make_variable(const char* name);
Node* make_operator(char op, Node* left, Node* right);

#endif
