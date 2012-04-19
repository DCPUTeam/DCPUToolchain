/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <libgen.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <argtable2.h>
#include "assem.h"
#include "node.h"
#include "aerr.h"
#include "pp.h"

extern int yyparse();
extern FILE *yyin, *yyout;
char* fileloc = NULL;

#define EMU_CMD_SIZE 80

#ifdef _WIN32
char* dirname(char* path)
{
	// FIXME: This assumes the resulting path will always
	// be shorter than the original (which it should be
	// given that we're only returning a component of it, right?)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(path, drive, dir, NULL, NULL);
	strcpy(path, drive);
	strcpy(path + strlen(path), dir);
	return path;
}
#endif

int main(int argc, char* argv[])
{
	FILE* img = NULL;
	char emucmd[EMU_CMD_SIZE];
	int nerrors, emures;
	struct errinfo* errval;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_lit* gen_relocatable = arg_lit0("r", "relocatable", "Generate relocatable code.");
	struct arg_lit* gen_intermediate = arg_lit0("i", "intermediate", "Generate intermediate code for use with the linker.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file (or - to read from standard input).");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_end *end = arg_end(20);
	void *argtable[] = { output_file, gen_relocatable, gen_intermediate, show_help, input_file, end };

	// Parse arguments.
	nerrors = arg_parse(argc,argv,argtable);
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "assembler");
		fprintf(stderr,"syntax:\n    assembler");
		arg_print_syntax(stdout, argtable, "\n");
		fprintf(stderr,"options:\n");
		arg_print_glossary(stdout, argtable, "    %-25s %s\n");
		exit(1);
	}

	// Set up error handling.
	errval = (struct errinfo*)setjmp(errjmp);
	if (errval != NULL)
	{
		// Handle the error.
		fprintf(stderr, "assembler: error occurred.\n");
		fprintf(stderr, err_strings[errval->errid], errval->errdata);
		if (img != NULL)
			fclose(img);
		return 1;
	}

	// Run the preprocessor.
	fileloc = malloc(strlen(input_file->filename[0]) + 1);
	memcpy(fileloc, input_file->filename[0], strlen(input_file->filename[0]) + 1);
	dirname(fileloc);
	pp_add_search_path(".");
	pp_add_search_path(fileloc);
	yyout = stderr;
	yyin = pp_do(input_file->filename[0]);
	if (yyin == NULL)
	{
		pp_cleanup();
		return 1;
	}

	// Parse assembly.
	yyparse();
	if (yyin != stdin)
		fclose(yyin);
	pp_cleanup();
	if (&ast_root == NULL || ast_root.values == NULL)
	{
		fprintf(stderr, "assembler: syntax error, see above.\n");
		return 1;
	}
	
	// Process AST.
	process_root(&ast_root);
	
	// Save to either file or stdout.
	if (strcmp(output_file->filename[0], "-") != 0)
	{
		// Write to file.
		img = fopen(output_file->filename[0], "wb");
		if (img == NULL)
		{
			fprintf(stderr, "assembler: output file not writable.\n");
			return 1;
		}
	}
	else
	{
		// Windows needs stdout in binary mode.
#ifdef _WIN32
		_setmode( _fileno( stdout ), _O_BINARY );
#endif

		// Set img to stdout.
		img = stdout;
	}
	
	// Write content.
	aout_write(img, (gen_relocatable->count > 0), (gen_intermediate->count > 0));
	fclose(img);
	fprintf(stderr, "assembler: completed successfully.\n");
	
	return 0;
}