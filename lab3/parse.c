#include "parse.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "tree.h"

typedef enum PrevToken {
    TOKEN_START,
    TOKEN_OPERAND,
    TOKEN_OPERATOR,
    TOKEN_LPAREN,
    TOKEN_RPAREN
} PrevToken;

int precedence(char op) {
    if (op == '~') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int is_right_associative(char op) {
    return op == '~';
}

int is_binary_operator(char op) {
    return op == '+' || op == '-' || op == '*' || op == '/';
}

void append_span(char* out, int* out_pos, const char* start, int length) {
    if (*out_pos > 0) {
        out[*out_pos] = ' ';
        (*out_pos)++;
    }

    memcpy(out + *out_pos, start, (size_t)length);
    *out_pos += length;
    out[*out_pos] = '\0';
}

void append_operator_token(char* out, int* out_pos, char op) {
    if (*out_pos > 0) {
        out[*out_pos] = ' ';
        (*out_pos)++;
    }

    out[*out_pos] = op;
    (*out_pos)++;
    out[*out_pos] = '\0';
}

char* convert_to_rpn(const char* expr, int* err_out) {
    size_t length = 0;
    char* output = NULL;
    const char* current = expr;
    int out_pos = 0;
    PrevToken prev = TOKEN_START;
    CharStack operators;

    if (err_out) *err_out = PARSE_OK;
    if (!expr) {
        if (err_out) *err_out = PARSE_NULL_INPUT;
        return NULL;
    }

    while (*current && isspace(*current)) current++;
    if (*current == '\0') {
        if (err_out) *err_out = PARSE_EMPTY_INPUT;
        return NULL;
    }

    length = strlen(expr);
    output = (char*)malloc(length * 4 + 32);
    if (!output) {
        if (err_out) *err_out = PARSE_MEMORY_ERROR;
        return NULL;
    }

    if (!init_char_stack(&operators, (int)length + 1)) {
        free(output);
        if (err_out) *err_out = PARSE_MEMORY_ERROR;
        return NULL;
    }

    output[0] = '\0';

    while (*current) {
        if (isspace(*current)) {
            current++;
            continue;
        }

        if (isdigit(*current)) {
            const char* start = current;

            if (prev == TOKEN_OPERAND || prev == TOKEN_RPAREN) {
                if (err_out) *err_out = PARSE_TRAILING_DATA;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            while (isdigit(*current)) current++;
            append_span(output, &out_pos, start, (int)(current - start));
            prev = TOKEN_OPERAND;
            continue;
        }

        if (isalpha(*current) || *current == '_') {
            const char* start = current;
            int id_length = 0;

            if (prev == TOKEN_OPERAND || prev == TOKEN_RPAREN) {
                if (err_out) *err_out = PARSE_TRAILING_DATA;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            while (isalpha(*current) ||
                   isdigit(*current) ||
                   *current == '_') {
                id_length++;
                current++;
            }

            if (id_length >= 32) {
                if (err_out) *err_out = PARSE_INVALID_TOKEN;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            append_span(output, &out_pos, start, id_length);
            prev = TOKEN_OPERAND;
            continue;
        }

        if (*current == '(') {
            if (prev == TOKEN_OPERAND || prev == TOKEN_RPAREN) {
                if (err_out) *err_out = PARSE_TRAILING_DATA;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            if (!push_char(&operators, '(')) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            prev = TOKEN_LPAREN;
            current++;
            continue;
        }

        if (*current == ')') {
            if (prev == TOKEN_OPERATOR || prev == TOKEN_LPAREN || prev == TOKEN_START) {
                if (err_out) *err_out = PARSE_EXPECTED_OPERAND;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            while (!char_stack_empty(&operators) && peek_char(&operators) != '(') {
                append_operator_token(output, &out_pos, pop_char(&operators));
            }

            if (char_stack_empty(&operators)) {
                if (err_out) *err_out = PARSE_MISSING_RPAREN;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            pop_char(&operators);
            prev = TOKEN_RPAREN;
            current++;
            continue;
        }

        if (is_binary_operator(*current)) {
            char op = *current;
            int unary = 0;

            if ((op == '+' || op == '-') &&
                (prev == TOKEN_START || prev == TOKEN_OPERATOR || prev == TOKEN_LPAREN)) {
                unary = 1;
            }

            if (unary && op == '+') {
                current++;
                continue;
            }

            if (!unary && (prev == TOKEN_START || prev == TOKEN_OPERATOR || prev == TOKEN_LPAREN)) {
                if (err_out) *err_out = PARSE_EXPECTED_OPERAND;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            if (unary && op == '-') {
                op = '~';
            }

            while (!char_stack_empty(&operators) &&
                   peek_char(&operators) != '(' &&
                   ((is_right_associative(op) && precedence(peek_char(&operators)) > precedence(op)) ||
                    (!is_right_associative(op) && precedence(peek_char(&operators)) >= precedence(op)))) {
                append_operator_token(output, &out_pos, pop_char(&operators));
            }

            if (!push_char(&operators, op)) {
                if (err_out) *err_out = PARSE_MEMORY_ERROR;
                free(output);
                free_char_stack(&operators);
                return NULL;
            }

            prev = TOKEN_OPERATOR;
            current++;
            continue;
        }

        if (err_out) *err_out = PARSE_INVALID_TOKEN;
        free(output);
        free_char_stack(&operators);
        return NULL;
    }

    if (prev == TOKEN_OPERATOR || prev == TOKEN_LPAREN || prev == TOKEN_START) {
        if (err_out) *err_out = PARSE_EXPECTED_OPERAND;
        free(output);
        free_char_stack(&operators);
        return NULL;
    }

    while (!char_stack_empty(&operators)) {
        char op = pop_char(&operators);

        if (op == '(') {
            if (err_out) *err_out = PARSE_MISSING_RPAREN;
            free(output);
            free_char_stack(&operators);
            return NULL;
        }

        append_operator_token(output, &out_pos, op);
    }

    free_char_stack(&operators);
    return output;
}

Node* parse_expression(const char* expr, int* err_out) {
    char* rpn = NULL;
    Node* root = NULL;

    rpn = convert_to_rpn(expr, err_out);
    if (!rpn) return NULL;

    root = build_tree_from_rpn(rpn, err_out);
    free(rpn);
    return root;
}
