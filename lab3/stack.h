#ifndef STACK_H
#define STACK_H

#include "structures.h"

typedef struct CharStack {
    char* data;
    int top;
    int capacity;
} CharStack;

typedef struct NodeStack {
    Node** data;
    int top;
    int capacity;
} NodeStack;

int init_char_stack(CharStack* stack, int capacity);
void free_char_stack(CharStack* stack);
int push_char(CharStack* stack, char value);
char pop_char(CharStack* stack);
char peek_char(CharStack* stack);
int char_stack_empty(CharStack* stack);

int init_node_stack(NodeStack* stack, int capacity);
void free_node_stack(NodeStack* stack);
int push_node(NodeStack* stack, Node* node);
Node* pop_node(NodeStack* stack);
Node* peek_node(NodeStack* stack);
int node_stack_size(NodeStack* stack);
void free_node_stack_items(NodeStack* stack);

#endif
