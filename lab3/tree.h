#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "structures.h"

Node* build_tree_from_rpn(const char* rpn, int* err_out);
Node* simplify_mul_by_zero(Node* root);
void print_tree(Node* root, int depth);
void print_expression(Node* root, FILE* out);
long long eval_expression(Node* root, int* err_out);
int has_mul_with_zero_operand(Node* root);
void free_tree(Node* root);

#endif
