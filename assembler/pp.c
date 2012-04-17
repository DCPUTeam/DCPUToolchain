/**

	File:			pp.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Handles preprocessing of input 
					files.

**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "posix.h"
#include "pp.h"
#include "dcpu.h"

#define INCLUDE_MAX 100

int pp_included_count = 0;
char* pp_included_names[INCLUDE_MAX];
int pp_path_count = 0;
const char* pp_path_names[PATH_COUNT_MAX];
void pp_base(FILE* in, FILE* out);

bool strsw(char* src, char* check)
{
	size_t l = strlen(check);
	char* buf = malloc(l + 1);
	int r;
	memcpy(buf, src, l);
	buf[l] = '\0';
	r = stricmp(buf, check);
	free(buf);
	return (r == 0);
}

void pp_init()
{
	int i = 0;
	for (i = 0; i < INCLUDE_MAX; i += 1)
		pp_included_names[i] = NULL;
}

void pp_add_search_path(const char* path)
{
	// Check to see whether we can't include any more paths.
	if (pp_included_count == PATH_COUNT_MAX)
	{
		fprintf(stderr, "preprocessor: maximum number of include paths (%i) reached, ignoring %s.\n", PATH_COUNT_MAX, path);
		return;
	}

	// Add the path.
	pp_path_names[pp_path_count] = malloc(strlen(path) + 1);
	memcpy(pp_path_names[pp_path_count++], path, strlen(path) + 1);
}

void pp_include(char* line, FILE* in, FILE* out)
{
	FILE* included = NULL;
	char* pos_a = strchr(line, '"');
	char* pos_b = strchr(pos_a + 1, '"');
	char* fname;
	char* cname;
	int path_i = 0;

	// Check to make sure the syntax is correct.
	if (pos_a == NULL || pos_b == NULL)
	{
		fprintf(stderr, "preprocessor: include directive '.%s' must use quotes.\n", line);
		return;
	}

	// Check to see whether we can't include any more files.
	if (pp_included_count == INCLUDE_MAX)
	{
		fprintf(stderr, "preprocessor: maximum number of includes (%i) reached.\n", INCLUDE_MAX);
		return;
	}

	// Get the filename component.
	fname = malloc(pos_b - pos_a + 1);
	strncpy(fname, pos_a + 1, pos_b - pos_a - 1);
	fname[pos_b - pos_a - 1] = '\0';

	// Search all of our paths for the file.
	while (included == NULL && path_i < pp_path_count)
	{
		// Calculate path.
		cname = malloc(strlen(pp_path_names[path_i]) + 1 + strlen(fname) + 1);
		memset(cname, 0, strlen(pp_path_names[path_i]) + 1 + strlen(fname) + 1);
		memcpy(cname, pp_path_names[path_i], strlen(pp_path_names[path_i]));
		memcpy(cname + strlen(pp_path_names[path_i]), "/", 1);
		memcpy(cname + strlen(pp_path_names[path_i]) + 1, fname, strlen(fname));

		// Attempt open.
		included = fopen(cname, "r");
		if (included == NULL)
			path_i++;
	}
	if (included == NULL)
		fprintf(stderr, "preprocessor: can not include %s.\n", fname);
	else
	{
		fprintf(stderr, "preprocessor: including %s.\n", fname);
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
		else if (current == '.' && has_newline)
		{
			// Start of preprocessor directive.
			line = fgets(line, 100, in);

			// Check to see what the preprocessor directive is.
			if (strsw(line, "INCLUDE"))
				pp_include(line, in, out);
			fputc('\n', out);
			
			// Reset variables.
			has_newline = true;
		}
		else if (current != EOF)
		{
			has_newline = false;
			fputc(current, out);
		}
	}
	while (current != EOF);

	fputc('\n', out);
}

char* pp_fname = NULL;

FILE* pp_do(const char* input)
{
	// Do preprocessing.
	FILE* in;
	FILE* out;
	if (pp_fname != NULL)
	{
		fprintf(stderr, "preprocessor: a preprocessing output file is already open.\n");
		return NULL;
	}
	pp_fname = tempnam(".", "pp.");
	if (strcmp(input, "-") == 0)
		in = stdin;
	else
		in = fopen(input, "r");
	if (in == NULL)
	{
		fprintf(stderr, "preprocessor: unable to read from input file.\n");
		return NULL;
	}
	out = fopen(pp_fname, "wb+");
	if (out == NULL)
	{
		if (strcmp(input, "-") != 0)
			fclose(in);
		fprintf(stderr, "preprocessor: unable to write to temporary output file '%s'.\n", pp_fname);
		return NULL;
	}
	pp_init();
	pp_base(in, out);
	if (strcmp(input, "-") != 0)
		fclose(in);
	fseek(out, 0, SEEK_SET);
	return out;
}

void pp_cleanup()
{
	remove(pp_fname);
	pp_fname = NULL;
}
