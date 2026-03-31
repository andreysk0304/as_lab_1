#include "tree.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "stack.h"
#include "structures.h"

int is_number_token(const char* token) {
    if (!token || !*token) return 0;

    for (const char* current = token; *current; current++) {
        if (!isdigit(*current)) return 0;
    }

    return 1;
}

int is_identifier_token(const char* token) {
    if (!token || !*token) return 0;
    if (!isalpha(token[0]) && token[0] != '_') return 0;

    for (const char* current = token + 1; *current; current++) {
        if (!isalpha(*current) &&
            !isdigit(*current) &&
            *current != '_') {
            return 0;
        }
    }

    return 1;
}

int is_zero_number(Node* node) {
    return node && node->kind == NODE_NUMBER && node->number == 0;
}

void free_tree(Node* root) {
    if (!root) return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

Node* build_tree_from_rpn(const char* rpn, int* err_out) {
    size_t length = 0;
    char* copy = NULL;
    char* token = NULL;
    NodeStack stack;

    if (err_out) *err_out = PARSE_OK;
    if (!rpn) {
        if (err_out) *err_out = PARSE_NULL_INPUT;
        return NULL;
    }

    while (*rpn && isspace(*rpn)) rpn++;
    if (*rpn == '\0') {
        if (err_out) *err_out = PARSE_EMPTY_INPUT;
        return NULL;
    }

    length = strlen(rpn);
    copy = (char*)malloc(length + 1);
    if (!copy) {
        if (err_out) *err_out = PARSE_MEMORY_ERROR;
        return NULL;
    }

    if (!init_node_stack(&stack, (int)length + 1)) {
        free(copy);
        if (err_out) *err_out = PARSE_MEMORY_ERROR;
        return NULL;
    }

    strcpy(copy, rpn);
    token = strtok(copy, " ");

    while (token) {
        if (strcmp(token, "~") == 0) {
            Node* operand = NULL;
            Node* zero = NULL;
            Node* node = NULL;

            if (node_stack_size(&stack) < 1) {
                if (err_out) *err_out = PARSE_EXPECTED_OPERAND;
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }

            operand = pop_node(&stack);
            zero = make_number(0);
            if (!zero) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(operand);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }

            node = make_operator('-', zero, operand);
            if (!node) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(zero);
                free_tree(operand);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }

            if (!push_node(&stack, node)) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(node);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }
        } else if (strlen(token) == 1 &&
                   (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/')) {
            Node* right = NULL;
            Node* left = NULL;
            Node* node = NULL;

            if (node_stack_size(&stack) < 2) {
                if (err_out) *err_out = PARSE_EXPECTED_OPERAND;
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }

            right = pop_node(&stack);
            left = pop_node(&stack);
            node = make_operator(token[0], left, right);
            if (!node) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(left);
                free_tree(right);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }

            if (!push_node(&stack, node)) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(node);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }
        } else if (is_number_token(token)) {
            Node* node = make_number(strtoll(token, NULL, 10));

            if (!node || !push_node(&stack, node)) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(node);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }
        } else if (is_identifier_token(token)) {
            Node* node = make_variable(token);

            if (!node || !push_node(&stack, node)) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free_tree(node);
                free_node_stack_items(&stack);
                free_node_stack(&stack);
                free(copy);
                return NULL;
            }
        } else {
            if (err_out) *err_out = PARSE_INVALID_TOKEN;
            free_node_stack_items(&stack);
            free_node_stack(&stack);
            free(copy);
            return NULL;
        }

        token = strtok(NULL, " ");
    }

    free(copy);

    if (node_stack_size(&stack) != 1) {
        if (err_out) *err_out = PARSE_TRAILING_DATA;
        free_node_stack_items(&stack);
        free_node_stack(&stack);
        return NULL;
    }

    Node* root = pop_node(&stack);
    free_node_stack(&stack);
    return root;
}

Node* simplify_mul_by_zero(Node* root) {
    if (!root) return NULL;

    if (root->kind != NODE_OPERATOR) return root;

    root->left = simplify_mul_by_zero(root->left);
    root->right = simplify_mul_by_zero(root->right);

    if (root->op == '*' && (is_zero_number(root->left) || is_zero_number(root->right))) {
        free_tree(root->left);
        free_tree(root->right);

        root->kind = NODE_NUMBER;
        root->number = 0;
        root->var[0] = '\0';
        root->op = 0;
        root->left = NULL;
        root->right = NULL;
    }

    return root;
}

int has_mul_with_zero_operand(Node* root) {
    if (!root) return 0;

    if (root->kind == NODE_OPERATOR && root->op == '*') {
        if (is_zero_number(root->left) || is_zero_number(root->right)) return 1;
    }

    return has_mul_with_zero_operand(root->left) || has_mul_with_zero_operand(root->right);
}

void print_tree(Node* root, int depth) {
    if (!root) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    if (root->kind == NODE_NUMBER) {
        printf("%lld\n", root->number);
    } else if (root->kind == NODE_VARIABLE) {
        printf("%s\n", root->var);
    } else {
        printf("%c\n", root->op);
    }

    print_tree(root->left, depth + 1);
    print_tree(root->right, depth + 1);
}

void print_expression(Node* root, FILE* out) {
    if (!root) return;

    if (root->kind == NODE_NUMBER) {
        fprintf(out, "%lld", root->number);
        return;
    }

    if (root->kind == NODE_VARIABLE) {
        fprintf(out, "%s", root->var);
        return;
    }

    fprintf(out, "(");
    print_expression(root->left, out);
    fprintf(out, " %c ", root->op);
    print_expression(root->right, out);
    fprintf(out, ")");
}

long long eval_expression(Node* root, int* err_out) {
    long long left = 0;
    long long right = 0;

    if (err_out) *err_out = 0;
    if (!root) {
        if (err_out) *err_out = 1;
        return 0;
    }

    if (root->kind == NODE_NUMBER) return root->number;

    if (root->kind == NODE_VARIABLE) {
        if (err_out) *err_out = 9;
        return 0;
    }

    left = eval_expression(root->left, err_out);
    if (err_out && *err_out) return 0;

    right = eval_expression(root->right, err_out);
    if (err_out && *err_out) return 0;

    switch (root->op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            if (right == 0) {
                if (err_out) *err_out = 2;
                return 0;
            }
            return left / right;
        default:
            if (err_out) *err_out = 3;
            return 0;
    }
}
