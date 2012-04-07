/**

	File:			pp.c

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Handles preprocessing of input 
					files.

**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pp.h"
#include "dcpu.h"

#define INCLUDE_MAX 100

int pp_included_count = 0;
char* pp_included_names[INCLUDE_MAX];
void pp_base(FILE* in, FILE* out);

bool strsw(char* src, char* check)
{
	size_t l = strlen(check);
	char* buf = malloc(l + 1);
	int r;
	memcpy(buf, src, l);
	buf[l] = '\0';
	r = strcmp(buf, check);
	free(buf);
	return (r == 0);
}

void pp_init()
{
	int i = 0;
	for (i = 0; i < INCLUDE_MAX; i += 1)
		pp_included_names[i] = NULL;
}

void pp_include(char* line, FILE* in, FILE* out)
{
	FILE* included;
	char* pos_a = strchr(line, '<');
	char* pos_b = strchr(line, '>');
	char* fname;

	// Check to make sure the syntax is correct.
	if (pos_a == NULL || pos_b == NULL)
	{
		printf("preprocessor: include directive '#%s' must use angled brackets.\n", line);
		return;
	}

	// Check to see whether we can't include any more files.
	if (pp_included_count == INCLUDE_MAX)
	{
		printf("preprocessor: maximum number of includes (%i) reached.\n", INCLUDE_MAX);
		return;
	}

	// Get the filename component.
	fname = malloc(pos_b - pos_a + 1);
	strncpy(fname, pos_a + 1, pos_b - pos_a - 1);
	fname[pos_b - pos_a - 1] = '\0';
	included = fopen(fname, "r");
	if (included == NULL)
		printf("preprocessor: can not include %s.\n", fname);
	else
	{
		printf("preprocessor: including %s.\n", fname);
		pp_included_names[pp_included_count++] = fname;
		pp_base(included, out);
	}
}

void pp_base(FILE* in, FILE* out)
{
	bool has_newline = false;
	char current = '\0';
	char* line = malloc(100); // Note: this means the preprocessor can only handle up to 100 characters of info.
	memset(line, 0, 100);

	do
	{
		current = fgetc(in);
		if (current == '\n')
		{
			has_newline = true;
			fputc(current, out);
		}
		else if (current == '#' && has_newline)
		{
			// Start of preprocessor directive.
			line = fgets(line, 100, in);

			// Check to see what the preprocessor directive is.
			if (strsw(line, "include"))
				pp_include(line, in, out);
			else
				printf("preprocessor: ignoring unknown directive #%s.\n", line);
			
			// Reset variables.
			has_newline = false;
		}
		else if (current != EOF)
		{
			has_newline = false;
			fputc(current, out);
		}
	}
	while (current != EOF);
}

char* pp_fname = NULL;

FILE* pp_do(const char* input)
{
	// Do preprocessing.
	FILE* in;
	FILE* out;
	if (pp_fname != NULL)
	{
		printf("preprocessor: a preprocessing output file is already open.\n");
		return NULL;
	}
	pp_fname = _tempnam(".", "pp.");
	in = fopen(input, "r");
	if (in == NULL)
	{
		printf("preprocessor: unable to read from input file.\n");
		return NULL;
	}
	out = fopen(pp_fname, "wb+");
	if (out == NULL)
	{
		fclose(in);
		printf("preprocessor: unable to write to temporary output file '%s'.\n", pp_fname);
		return NULL;
	}
	pp_init();
	pp_base(in, out);
	fclose(in);
	fseek(out, 0, SEEK_SET);
	return out;
}

void pp_cleanup()
{
	remove(pp_fname);
	pp_fname = NULL;
}