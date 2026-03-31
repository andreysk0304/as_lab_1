#ifndef PARSE_H
#define PARSE_H

#include "structures.h"

enum ParseError {
    PARSE_OK = 0,
    PARSE_NULL_INPUT = 1,
    PARSE_EMPTY_INPUT = 2,
    PARSE_MEMORY_ERROR = 3,
    PARSE_EXPECTED_OPERAND = 4,
    PARSE_MISSING_RPAREN = 5,
    PARSE_INVALID_TOKEN = 6,
    PARSE_TRAILING_DATA = 7
};

char* convert_to_rpn(const char* expr, int* err_out);
Node* parse_expression(const char* expr, int* err_out);

#endif
