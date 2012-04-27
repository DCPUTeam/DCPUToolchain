/**

	File:			pp.c

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Defines the public API for using the preprocessor
					inline in programs.

**/

#include <stdio.h>
#include <bstrlib.h>
#include "parser.h"
#include "lexer.h"
#include "pp.h"

extern int pp_yyparse(void* scanner);

bstring pp_do(bstring path)
{
	FILE* in;
	FILE* out;
	bstring temp;
	yyscan_t scanner;

	// Open and set up the temporary output areas.
	temp = bfromcstr(tempnam(".", "pp."));
	if (biseq(path, bfromcstr("-")))
		in = stdin;
	else
		in = fopen((const char*)(path->data), "r");
	if (in == NULL)
	{
		fprintf(stderr, "preprocessor: unable to read from input file.\n");
		return NULL;
	}
	out = fopen((const char*)(temp->data), "w");
	if (out == NULL)
	{
		if (!biseq(path, bfromcstr("-")))
			fclose(in);
		fprintf(stderr, "preprocessor: unable to write to temporary output file '%s'.\n", temp->data);
		return NULL;
	}

	// Now invoke the preprocessor's lexer and parser.
	pp_yylex_init(&scanner);
	pp_yyset_out(out, scanner);
	pp_yyset_in(in, scanner);
	pp_yyparse(scanner);
	pp_yylex_destroy(scanner);

	// Now do some cleanup.
	if (!biseq(path, bfromcstr("-")))
		fclose(in);
	fseek(out, 0, SEEK_SET);

	// Return the filename.
	return temp;
}

void pp_cleanup(bstring path)
{
	if (path == NULL) return;
	remove((const char*)(path->data));
}