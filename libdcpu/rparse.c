/**
 *
 *	File:		rparse.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Defines functions for parsing line and filename
 *			information at a lexing stage.
 *
 **/

#include <stdlib.h>
#include <stdio.h>
#include "bstring.h"
#include "rparse.h"

void rparse_do(char* yytext, int* out_line, bstring* out_filename)
{
	bstring str = bfromcstr(yytext);
	bstring space = bfromcstr(" ");

	// Determine the space positions.
	int firstSpace = binstr(str, 0, space);
	int secondSpace = binstr(str, firstSpace + 1, space);

	// Create substrings.
	bstring bline = bmidstr(str, firstSpace + 1, secondSpace - firstSpace);
	bstring file = bmidstr(str, secondSpace + 1, blength(str) - secondSpace);

	// Convert the line number and trim file name.
	int line = strtoul((const char*)bline->data, NULL, 10);
	btrimws(file);

	// Set variables.
	if (*out_filename == NULL)
		*out_filename = bfromcstr("");
	bassign(*out_filename, file);
	*out_line = line;

	// Free resources.
	bdestroy(file);
	bdestroy(bline);
	bdestroy(space);
	bdestroy(str);
}

