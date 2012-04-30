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
#include "node.h"
#include "imap.h"

extern unsigned int yyalineno;

// Root node for the AST.
struct ast_node_root ast_root;

int yylex();

void yyerror(const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", yyalineno, str);
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
	int* token;
}

// Define our lexical token names.
%token <token> COMMA BRACKET_OPEN BRACKET_CLOSE COLON SEMICOLON NEWLINE COMMENT ADD
%token <token> KEYWORD BOUNDARY EXTENSION ORIGIN INCLUDE INCBIN EXPORT IMPORT ERROR EQUATE FILL
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
		EQUATE WORD parameters NEWLINE
		{
			// Ugh, this is such an ugly hack and really needs to be better.
			struct ast_node_instruction* linst = malloc(sizeof(struct ast_node_instruction));
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			struct ast_node_label* lbl = malloc(sizeof(struct ast_node_label));

			// Initialize DAT instruction.
			linst->instruction = "DAT";
			linst->parameters = $3;

			// Initialize label.
			lbl->name = $2;

			// Initialize instruction.
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = lbl;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->keyword = 0;
			$$->instruction = linst;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = 0;
		} |
		FILL ADDRESS ADDRESS NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = $2;
			$$->keyword_data_numeric_2 = $3;
		} |
		KEYWORD WORD NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = $2;
			$$->keyword_data_numeric = 0;
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
			$$->keyword_data_numeric = 0;
		} |
		KEYWORD ADDRESS NEWLINE
		{
			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$1;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = NULL;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = $2;
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
			$$->keyword_data_numeric = 0;
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
			$$->keyword_data_numeric = 0;
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
			$$->keyword_data_numeric = 0;
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
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_instruction;
			$$->keyword = 0;
			$$->instruction = $2;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = 0;
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
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = $3;
			$$->keyword_data_numeric = 0;
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
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = $3;
			$$->keyword_data_numeric = 0;
		} |
		label KEYWORD ADDRESS NEWLINE
		{
			struct ast_node_line* lnode = malloc(sizeof(struct ast_node_line));
			lnode->type = type_label;
			lnode->keyword = 0;
			lnode->instruction = NULL;
			lnode->label = $1;
			lnode->prev = NULL;
			lnode->keyword_data_string = NULL;
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = $3;
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
			lnode->keyword_data_numeric = 0;

			$$ = malloc(sizeof(struct ast_node_line));
			$$->type = type_keyword;
			$$->keyword = (long)$2;
			$$->instruction = NULL;
			$$->label = NULL;
			$$->prev = lnode;
			$$->keyword_data_string = NULL;
			$$->keyword_data_numeric = 0;
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
			strupper($1);
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
		} |
		BRACKET_OPEN WORD ADD ADDRESS BRACKET_CLOSE
		{
			$$ = malloc(sizeof(struct ast_node_address));
			$$->value = $4;
			$$->bracketed = 1;
			$$->added = 1;
			$$->addcmpt = $2;
		};
