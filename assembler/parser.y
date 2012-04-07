%{

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

extern unsigned int yylineno;

// Root node for the AST.
struct ast_node_root ast_root;
 
void yyerror(const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", yylineno, str);
}
 
int yywrap()
{
    return 1;
} 

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	char* string;
	struct ast_node_address* address;
	struct ast_node_register* registr;
	struct ast_node_parameter* parameter;
	struct ast_node_parameters* parameters;
	struct ast_node_instruction* instruction;
	struct ast_node_label* label;
	struct ast_node_line* line;
	struct ast_node_lines* lines;
	struct ast_node_root* root;
}

// Define our lexical token names.
%token <token> COMMA BRACKET_OPEN BRACKET_CLOSE COLON SEMICOLON NEWLINE COMMENT ADD
%token <string> WORD STRING CHARACTER
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
		instruction NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->instruction = $1;
			$$->label = NULL;
			$$->prev = NULL;
		} |
		label NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_label;
			$$->instruction = NULL;
			$$->label = $1;
			$$->prev = NULL;
		} |
		label instruction NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->instruction = $2;
			$$->label = NULL;
			$$->prev = lnode;
		} ;

label:
		COLON WORD
		{
			$$ = malloc(sizeof(struct ast_node_label));
			$$->name = $2;
		} ;

instruction:
		WORD parameters
		{
			$$ = malloc(sizeof(struct ast_node_instruction));
			$$->instruction = $1;
			$$->parameters = $2;
		} ;

parameters:
		{
			$$ = NULL;
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
		WORD
		{
			$$ = malloc(sizeof(struct ast_node_register));
			$$->value = $1;
			$$->bracketed = 0;
		};

bracketed_register:
		BRACKET_OPEN WORD BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_register));
			$$->value = $2;
			$$->bracketed = 1;
		};

address:
		ADDRESS
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $1;
			$$->bracketed = 0;
			$$->added = 0;
			$$->addcmpt = NULL;
		} |
		CHARACTER
		{
			$$ = malloc(sizeof(struct ast_node_parameter));
			$$->value = (uint16_t)($1[0]);
			$$->bracketed = 0;
			$$->added = 0;
			$$->addcmpt = NULL;
		};

bracketed_address:
		BRACKET_OPEN ADDRESS BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $2;
			$$->bracketed = 1;
			$$->added = 0;
			$$->addcmpt = NULL;
		};

bracketed_added_address:
		BRACKET_OPEN ADDRESS ADD WORD BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $2;
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $4;
		};