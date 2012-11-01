/**
 *
 *  File:       ppexpr.h
 *
 *  Project:    DCPU-16 Toolchain
 *  Component:  LibDCPU-PP-Expr
 *
 *  Authors:    James Rhodes
 *
 *  Description:    Declares the public API for using the compile-time
 *          expression system in programs.
 *
 **/

#ifndef __DCPU_LIBDCPU_PP_EXPR_H
#define __DCPU_LIBDCPU_PP_EXPR_H

#include <bstring.h>
#include <stdint.h>

struct expr
{
    int type;
    struct expr* a;
    int op;
    struct expr* b;
    void* data;
};

// Expression types
#define EXPR_EXPR       0
#define EXPR_NUMBER     1
#define EXPR_LABEL      2

// Expression operations
#define EXPR_OP_INVALID     0
#define EXPR_OP_ADD     1
#define EXPR_OP_SUBTRACT    2
#define EXPR_OP_MULTIPLY    3
#define EXPR_OP_DIVIDE      4
#define EXPR_OP_MODULUS     5
#define EXPR_OP_EQUALS      6
#define EXPR_OP_NOT_EQUALS  7
#define EXPR_OP_LESS_THAN   8
#define EXPR_OP_LESS_EQUALS 9
#define EXPR_OP_GREATER_THAN    10
#define EXPR_OP_GREATER_EQUALS  11
#define EXPR_OP_AND     12
#define EXPR_OP_BOR     13
#define EXPR_OP_XOR     14
#define EXPR_OP_BOOLEAN_NOT 15
#define EXPR_OP_BOOLEAN_AND 16
#define EXPR_OP_BOOLEAN_OR 17
#define EXPR_OP_BOOLEAN_XOR 18
#define EXPR_OP_LEFT_SHIFT 19
#define EXPR_OP_RIGHT_SHIFT 20

// Expression exit codes
#define EXPR_EXIT_LABEL_NOT_FOUND   0
#define EXPR_EXIT_DIVIDE_BY_ZERO    1

struct expr* expr_new(struct expr* a, int op, struct expr* b);
struct expr* expr_new_number(uint16_t val);
struct expr* expr_new_label(freed_bstring label);
uint16_t expr_evaluate(struct expr* e, uint16_t(*labelreplace)(bstring label), void(*errexit)(int code, void* data));
bstring expr_representation(struct expr* e);
void expr_delete(struct expr* e);
struct expr* expr_parse(bstring input);

#endif
