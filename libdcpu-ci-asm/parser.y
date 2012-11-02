%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      Assembler

	Authors:        James Rhodes

	Description:    Defines parser for the assembler.

**/

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bstring.h>
#include <ppexpr.h>
#include <debug.h>
#include <assert.h>
#include <derr.h>
#include "node.h"
#include "imap.h"

// Root node for the AST.
struct ast_node_root ast_root;

extern int yylex();
void yyerror(const char *str);

// Filename tracking for local assembly.
extern int	yylineno;
extern bstring	yyfilename;

// Error handling.
void yyerror(const char *str)
{
    if (yyfilename == NULL)
    	printd(LEVEL_ERROR, "error at line %i of '%s': %s\n", yylineno, "<unknown>", str);
    else
        printd(LEVEL_ERROR, "error at line %i of '%s': %s\n", yylineno, yyfilename->data, str);
}

// Filename tracking for language producing assembly.
extern int	yyulineno;
extern bstring	yyufilename;

#define NODE_SET_GLOBALS(node) \
	node->line = yylineno; \
	node->file = bstrcpy(yyfilename); \
	node->uline = yyulineno; \
	if (yyufilename != NULL) \
		node->ufile = bstrcpy(yyufilename); \
	else \
		node->ufile = NULL; \
	yyufilename = NULL;

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	char* word;
	bstring string;
	struct ast_node_address* address;
	struct ast_node_register* registr;
	struct ast_node_parameter* parameter;
	struct ast_node_parameters* parameters;
	struct ast_node_instruction* instruction;
	struct ast_node_label* label;
	struct ast_node_line* line;
	struct ast_node_lines* lines;
	struct ast_node_root* root;
	struct expr* expr;
	int* token;
}

// Define our lexical token names.
%token <token> COMMA BRACKET_OPEN BRACKET_CLOSE COLON SEMICOLON NEWLINE COMMENT
%token <token> ADD SUBTRACT MULTIPLY DIVIDE MODULUS EQUALS NOT_EQUALS LESS_THAN LESS_EQUALS GREATER_THAN GREATER_EQUALS
%token <token> PAREN_OPEN PAREN_CLOSE BITWISE_AND BITWISE_BOR BITWISE_XOR BITWISE_NOT BOOLEAN_OR BOOLEAN_AND BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <token> LEX_PICK KEYWORD BOUNDARY EXTENSION ORIGIN INCLUDE INCBIN EXPORT IMPORT ERROR EQUATE FILL SECTION OUTPUT SYMBOL SEEK JUMP IMPORT_OPTIONAL
%token <word> WORD REGISTER
%token <string> STRING CHARACTER
%token <number> ADDRESS

// Defines the type of each parser symbols.
%type <address> bracketed_added_address
%type <address> bracketed_address
%type <address> address
%type <registr> bracketed_register
%type <registr> register
%type <parameter> parameter
%type <parameters> parameters
%type <instruction> instruction
%type <label> label
%type <line> line
%type <lines> lines
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
%start root

%%

root:
		lines
		{
			ast_root.values = $1;
		} ;

lines:
		line
		{
			$$ = malloc(sizeof(struct ast_node_lines));
			$$->last = $1;
		} |
		lines line
		{
			if ($2 != NULL)
			{
				// Check to see whether prev is already set
				// (in the case of label + instruction).
				if ($2->prev != NULL)
				{
					if ($1->last != NULL)
						$2->prev->prev = $1->last;
				}
				else
				{
					if ($1->last != NULL)
						$2->prev = $1->last;
				}
				$1->last = $2;
			}
		} ;

line:
		{
			$$ = NULL;
		} |
		NEWLINE
		{
			$$ = NULL;
		} |
		FILL expr expr NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = $2;
			$$->keyword_data_expr_2 = $3;
			NODE_SET_GLOBALS($$);
		} |
		KEYWORD IMPORT_OPTIONAL WORD NEWLINE
		{
            /*if ((long)$1 != IMPORT)
                dhalt(ERR_ONLY_IMPORT_OPTIONAL, NULL);*/
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)IMPORT_OPTIONAL;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = bfromcstr($3);
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
        KEYWORD WORD NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = bfromcstr($2);
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		KEYWORD STRING NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = $2;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		KEYWORD expr NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = $2;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		KEYWORD NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		instruction NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->keyword = 0;
			$$->instruction = $1;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		label NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_label;
			$$->keyword = 0;
			$$->instruction = NULL;
			$$->label = $1;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		label instruction NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_expr_1 = NULL;
			lnode->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS(lnode);

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->keyword = 0;
			$$->instruction = $2;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		label KEYWORD WORD NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_expr_1 = NULL;
			lnode->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS(lnode);

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = bfromcstr($3);
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS($$);
		} |
		label KEYWORD STRING NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_expr_1 = NULL;
			lnode->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS(lnode);

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = $3;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			$$->line = yylineno;
			$$->file = bstrcpy(yyfilename);
		} |
		label KEYWORD expr NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_expr_1 = NULL;
			lnode->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS(lnode);

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = $3;
			$$->keyword_data_expr_2 = NULL;
			$$->line = yylineno;
			$$->file = bstrcpy(yyfilename);
		} |
		label KEYWORD NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_expr_1 = NULL;
			lnode->keyword_data_expr_2 = NULL;
			NODE_SET_GLOBALS(lnode);

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_expr_1 = NULL;
			$$->keyword_data_expr_2 = NULL;
			$$->line = yylineno;
			$$->file = bstrcpy(yyfilename);
		} ;

label:
		COLON WORD
		{
			$$ = malloc(sizeof(struct ast_node_label));
			$$->name = $2;
		} |
		WORD COLON
		{
			$$ = malloc(sizeof(struct ast_node_label));
			$$->name = $1;
		} ;

instruction:
		WORD parameters
		{
			$$ = malloc(sizeof(struct ast_node_instruction));
			strupper($1);
			$$->instruction = $1;
			$$->parameters = $2;
		} ;

parameters:
		{
			$$ = malloc(sizeof(struct ast_node_parameters));
			$$->last = NULL;
		} |
		parameter
		{
			$$ = malloc(sizeof(struct ast_node_parameters));
			$$->last = $1;
		} |
		parameters COMMA parameter
		{
			if ($3 != NULL)
			{
				if ($1->last != NULL)
					$3->prev = $1->last;
				$1->last = $3;
			}
			$$ = $1;
		} ;

parameter:
		register
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_register;
			$$->registr = $1;
			$$->address = NULL;
			$$->raw = NULL;
			$$->prev = NULL;
		} | 
		bracketed_register
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_register;
			$$->registr = $1;
			$$->address = NULL;
			$$->raw = NULL;
			$$->prev = NULL;
		} |
		address
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_address;
			$$->registr = NULL;
			$$->address = $1;
			$$->raw = NULL;
			$$->prev = NULL;
		} |
		bracketed_address
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_address;
			$$->registr = NULL;
			$$->address = $1;
			$$->raw = NULL;
			$$->prev = NULL;
		} |
		bracketed_added_address
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_address;
			$$->registr = NULL;
			$$->address = $1;
			$$->raw = NULL;
			$$->prev = NULL;
		} |
		STRING
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->type = type_raw;
			$$->registr = NULL;
			$$->address = NULL;
			$$->raw = $1;
			$$->prev = NULL;
		} ;

register:
		REGISTER
		{
			$$ = malloc(sizeof(struct ast_node_register));
			$$->value = $1;
			$$->bracketed = 0;
		};

bracketed_register:
		BRACKET_OPEN REGISTER BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_register));
			$$->value = $2;
			$$->bracketed = 1;
		};

address:
		LEX_PICK expr /* PICK expr */
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $2;
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = "SP";
		} |
		expr
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $1;
			$$->bracketed = 0;
			$$->added = 0;
			$$->addcmpt = NULL;
		};

bracketed_address:
		BRACKET_OPEN expr BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $2;
			$$->bracketed = 1;
			$$->added = 0;
			$$->addcmpt = NULL;
		};

bracketed_added_address:
		BRACKET_OPEN expr ADD REGISTER BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $2;
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $4;
		} |
		BRACKET_OPEN REGISTER ADD expr BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $4;
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $2;
		} |
		BRACKET_OPEN expr SUBTRACT REGISTER BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = expr_new(expr_new_number(0x0), EXPR_OP_SUBTRACT, $2);
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $4;
		} |
		BRACKET_OPEN REGISTER SUBTRACT expr BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = expr_new(expr_new_number(0x0), EXPR_OP_SUBTRACT, $4);
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $2;
		};

expr:
		ADDRESS
		{
			$$ = expr_new_number($1);
		} |
		CHARACTER
		{
			$$ = expr_new_number((uint16_t)$1->data[0]);
			bdestroy($1);
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
