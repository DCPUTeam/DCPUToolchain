/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Preprocessor

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#include <stdio.h>
#include <bstring.h>
#include <simclist.h>
#include <parser.h>
#include <lexer.h>
#include <assert.h>
#include <ppfind.h>

extern int pp_yyparse(void* scanner);

int main(int argc, char* argv[])
{
	yyscan_t scanner;
	FILE* file;

	file = fopen(argv[1], "r");
	assert(file != NULL);
	ppfind_add_autopath(bautofree(bfromcstr(argv[1])));
	pp_yylex_init(&scanner);
	pp_yyset_out(stdout, scanner);
	pp_yyset_in(file, scanner);
	pp_yyparse(scanner);
	pp_yylex_destroy(scanner);
	return 0;
}
