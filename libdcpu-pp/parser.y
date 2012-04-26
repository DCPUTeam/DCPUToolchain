%{
	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Defines parser for the preprocessor.

**/

#define YYDEBUG 1
#define YYERROR_VERBOSE

#include <stdio.h>
FILE* yyout;
extern int yylineno;

void yyerror(const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", yylineno, str);
}

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	char* string;
	char any;
	int* token;
}

// Define our lexical token names.
%token <token> INCLUDE EQUATE
%token <string> STRING WORD
%token <any> ANY_CHAR
%token <number> NUMBER

// Defines the type of each parser symbols.


// Start at the root node.
%start root

%%

root:
		lines;

lines:
		line |
		lines line ;

line:
		preprocessor |
		text ;

preprocessor:
		INCLUDE STRING
		{
			fprintf(yyout, "(include directive)");
		} |
		EQUATE WORD STRING
		{
			fprintf(yyout, "(equate directive %s -> %s)", $2, $3);
		} ;

text:
		ANY_CHAR
		{
			fprintf(yyout, "%c", $1);
		} | 
		text ANY_CHAR
		{
			fprintf(yyout, "%c", $2);
		} ;
