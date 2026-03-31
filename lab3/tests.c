#include "parse.h"
#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long eval_or_fail(Node* root) {
    int error = 0;
    long long value = eval_expression(root, &error);

    assert(error == 0);
    return value;
}

void assert_number(Node* node, long long expected) {
    assert(node != NULL);
    assert(node->kind == NODE_NUMBER);
    assert(node->number == expected);
}

void assert_variable(Node* node, const char* expected) {
    assert(node != NULL);
    assert(node->kind == NODE_VARIABLE);
    assert(strcmp(node->var, expected) == 0);
}

void assert_operator(Node* node, char expected) {
    assert(node != NULL);
    assert(node->kind == NODE_OPERATOR);
    assert(node->op == expected);
}

// Проверяет перевод в ОПН с учётом приоритета операций.
void test_rpn_with_precedence(void) {
    printf("test 1, rpn with precedence...\n");
    int error = 0;
    char* rpn = convert_to_rpn("2+3*4", &error);

    assert(rpn != NULL);
    assert(error == 0);
    assert(strcmp(rpn, "2 3 4 * +") == 0);

    free(rpn);
    printf("test 1 complete\n");
}

// Проверяет, что дерево строится с правильным приоритетом операций.
void test_tree_build_with_precedence(void) {
    printf("test 2, tree build with precedence...\n");

    int error = 0;
    Node* root = parse_expression("2+3*4", &error);

    assert(root != NULL);
    assert(error == 0);
    assert_operator(root, '+');
    assert_number(root->left, 2);
    assert_operator(root->right, '*');
    assert_number(root->right->left, 3);
    assert_number(root->right->right, 4);
    assert(eval_or_fail(root) == 14);

    free_tree(root);
    printf("test 2 complete\n");
}

// Проверяет упрощение одного произведения с нулём.
void test_single_zero_multiplication(void) {
    printf("test 3, single zero multiplication...\n");

    int error = 0;
    Node* root = parse_expression("5*0+2", &error);

    assert(root != NULL);
    assert(error == 0);

    simplify_mul_by_zero(root);

    assert_operator(root, '+');
    assert_number(root->left, 0);
    assert_number(root->right, 2);
    assert(has_mul_with_zero_operand(root) == 0);
    assert(eval_or_fail(root) == 2);

    free_tree(root);
    printf("test 3 complete\n");
}

// Проверяет упрощение нескольких произведений с нулём в разных местах дерева.
void test_many_zero_multiplications(void) {
    printf("test 4, many zero multiplications...\n");

    int error = 0;
    Node* root = parse_expression("a*0 + (b*(c*0))", &error);

    assert(root != NULL);
    assert(error == 0);

    simplify_mul_by_zero(root);

    assert_operator(root, '+');
    assert_number(root->left, 0);
    assert_number(root->right, 0);
    assert(has_mul_with_zero_operand(root) == 0);

    free_tree(root);
    printf("test 4 complete\n");
}

// Проверяет перевод унарного минуса в ОПН.
void test_rpn_with_unary_minus(void) {
    printf("test 5, rpn with unary minus...\n");

    int error = 0;
    char* rpn = convert_to_rpn("2*-3", &error);

    assert(rpn != NULL);
    assert(error == 0);
    assert(strcmp(rpn, "2 3 ~ *") == 0);

    free(rpn);
    printf("test 5 complete\n");
}

// Проверяет построение дерева для выражения с унарным минусом после умножения.
void test_unary_minus_after_multiplication(void) {
    printf("test 6, unary minus after multiplication...\n");

    int error = 0;
    Node* root = parse_expression("2*-3", &error);

    assert(root != NULL);
    assert(error == 0);
    assert_operator(root, '*');
    assert_number(root->left, 2);
    assert_operator(root->right, '-');
    assert_number(root->right->left, 0);
    assert_number(root->right->right, 3);
    assert(eval_or_fail(root) == -6);

    free_tree(root);
    printf("test 6 complete\n");
}

// Проверяет корректный разбор двойного минуса.
void test_double_minus(void) {
    printf("test 7, double minus...\n");

    int error = 0;
    char* rpn = convert_to_rpn("x--0", &error);

    assert(rpn != NULL);
    assert(error == 0);
    assert(strcmp(rpn, "x 0 ~ -") == 0);
    free(rpn);

    error = 0;
    Node* root = parse_expression("x--0", &error);

    assert(root != NULL);
    assert(error == 0);
    assert_operator(root, '-');
    assert_variable(root->left, "x");
    assert_operator(root->right, '-');
    assert_number(root->right->left, 0);
    assert_number(root->right->right, 0);

    free_tree(root);
    printf("test 7 complete\n");
}

// Проверяет, что выражение с унарным минусом и умножением на ноль сводится к нулю.
void test_unary_minus_with_zero_multiplication(void) {
    printf("test 8, unary minus with zero multiplication...\n");

    int error = 0;
    Node* root = parse_expression("-x*0", &error);

    assert(root != NULL);
    assert(error == 0);

    simplify_mul_by_zero(root);

    assert_number(root, 0);
    assert(has_mul_with_zero_operand(root) == 0);

    free_tree(root);
    printf("test 8 complete\n");
}

int main(void) {
    printf("STARTING LAB3 TESTS...\n\n");

    test_rpn_with_precedence();
    test_tree_build_with_precedence();
    test_single_zero_multiplication();
    test_many_zero_multiplications();
    test_rpn_with_unary_minus();
    test_unary_minus_after_multiplication();
    test_double_minus();
    test_unary_minus_with_zero_multiplication();

    printf("\nALL TESTS PASSED\n");
    return 0;
}
