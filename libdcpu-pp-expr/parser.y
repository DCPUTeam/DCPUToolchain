%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-PP-Expr

	Authors:        James Rhodes

	Description:    Defines parser for the expression parser.

**/

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bstring.h>
#include "ppexpr.h"

extern int yylex();
void ppexpr_yyerror(struct expr** expr, void* scanner, const char* str);

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	char* word;
	struct expr* expr;
	int* token;
}

// Define our lexical token names.
%token <token> ADD SUBTRACT MULTIPLY DIVIDE MODULUS EQUALS NOT_EQUALS LESS_THAN LESS_EQUALS GREATER_THAN GREATER_EQUALS
%token <token> PAREN_OPEN PAREN_CLOSE BITWISE_AND BITWISE_BOR BITWISE_XOR BITWISE_NOT BOOLEAN_OR BOOLEAN_AND BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <word> WORD
%token <number> ADDRESS

// Defines the type of each parser symbols.
%type <expr> expr

// Operator precedence (lowest -> highest)
%left BOOLEAN_OR
%left BOOLEAN_AND
%left BINARY_OR
%left BINARY_XOR
%left BINARY_AND
%left COMPARE_EQUAL COMPARE_NOT_EQUAL
%left COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
%left BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%left ADD SUBTRACT
%left MULTIPLY DIVIDE MODULUS

// Start at the root node.
%start input

// Set parser name.
%pure-parser
%name-prefix="ppexpr_yy"
%lex-param { void* scanner }
%parse-param { struct expr** result }
%parse-param { void* scanner }

%%

input:		expr
		{
			*result = $1;
		} ;

expr:
		ADDRESS
		{
			$$ = expr_new_number($1);
		} |
		WORD
		{
			$$ = expr_new_label(bautofree(bfromcstr($1)));
			free($1);
		} |
		PAREN_OPEN expr PAREN_CLOSE
		{
			$$ = $2;
		} |
		SUBTRACT expr
		{
			$$ = expr_new(expr_new_number(0), EXPR_OP_SUBTRACT, $2);
		} |
		BITWISE_NOT expr
		{
			$$ = expr_new(expr_new_number(0xFFFF), EXPR_OP_XOR, $2);
		} |
		expr DIVIDE expr
		{
			$$ = expr_new($1, EXPR_OP_DIVIDE, $3);
		} |
		expr MULTIPLY expr
		{
			$$ = expr_new($1, EXPR_OP_MULTIPLY, $3);
		} |
		expr SUBTRACT expr
		{
			$$ = expr_new($1, EXPR_OP_SUBTRACT, $3);
		} |
		expr ADD expr
		{
			$$ = expr_new($1, EXPR_OP_ADD, $3);
		} |
		expr MODULUS expr
		{
			$$ = expr_new($1, EXPR_OP_MODULUS, $3);
		} |
		expr BITWISE_AND expr
		{
			$$ = expr_new($1, EXPR_OP_AND, $3);
		} |
		expr BITWISE_XOR expr
		{
			$$ = expr_new($1, EXPR_OP_XOR, $3);
		} |
		expr BITWISE_BOR expr
		{
			$$ = expr_new($1, EXPR_OP_BOR, $3);
		} |
		expr EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_EQUALS, $3);
		} |
		expr NOT_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_NOT_EQUALS, $3);
		} |
		expr LESS_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_THAN, $3);
		} |
		expr LESS_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_EQUALS, $3);
		} |
		expr GREATER_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_THAN, $3);
		} |
		expr GREATER_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_EQUALS, $3);
		} ;

%%

void ppexpr_yyerror(struct expr** expr, void* scanner, const char* str)
{
	// TODO: Do something appropriate here.
}