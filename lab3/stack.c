#include "stack.h"

#include <stdlib.h>

#include "tree.h"

int init_char_stack(CharStack* stack, int capacity) {
    stack->data = (char*)malloc((size_t)capacity);
    if (!stack->data) return 0;

    stack->top = -1;
    stack->capacity = capacity;
    return 1;
}

void free_char_stack(CharStack* stack) {
    free(stack->data);
    stack->data = NULL;
    stack->top = -1;
    stack->capacity = 0;
}

int push_char(CharStack* stack, char value) {
    if (stack->top + 1 >= stack->capacity) return 0;

    stack->top++;
    stack->data[stack->top] = value;
    return 1;
}

char pop_char(CharStack* stack) {
    char value = stack->data[stack->top];
    stack->top--;
    return value;
}

char peek_char(CharStack* stack) {
    return stack->data[stack->top];
}

int char_stack_empty(CharStack* stack) {
    return stack->top < 0;
}

int init_node_stack(NodeStack* stack, int capacity) {
    stack->data = (Node**)malloc(sizeof(Node*) * (size_t)capacity);
    if (!stack->data) return 0;

    stack->top = -1;
    stack->capacity = capacity;
    return 1;
}

void free_node_stack(NodeStack* stack) {
    free(stack->data);
    stack->data = NULL;
    stack->top = -1;
    stack->capacity = 0;
}

int push_node(NodeStack* stack, Node* node) {
    if (stack->top + 1 >= stack->capacity) return 0;

    stack->top++;
    stack->data[stack->top] = node;
    return 1;
}

Node* pop_node(NodeStack* stack) {
    Node* node = stack->data[stack->top];
    stack->top--;
    return node;
}

Node* peek_node(NodeStack* stack) {
    return stack->data[stack->top];
}

int node_stack_size(NodeStack* stack) {
    return stack->top + 1;
}

void free_node_stack_items(NodeStack* stack) {
    for (int i = 0; i <= stack->top; i++) {
        free_tree(stack->data[i]);
    }
}
