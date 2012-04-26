/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Preprocessor

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#include <stdio.h>
#include <bstrlib.h>
#include "parser.h"

extern int yydebug;
extern int yyparse();
extern FILE *yyin, *yyout;

int main(int argc, char* argv[])
{
	//yydebug = 1;
	yyout = stdout;
	yyin = fopen(argv[1], "rb");
	yyparse();
	return 0;
}