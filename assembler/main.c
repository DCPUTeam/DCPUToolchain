/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <argtable2.h>
#include <bstring.h>
#include <osutil.h>
#include <pp.h>
#include <ppfind.h>
#include "assem.h"
#include "node.h"
#include "aerr.h"
#include "aout.h"

extern int yyparse();
extern FILE *yyin, *yyout;
char* fileloc = NULL;

#define EMU_CMD_SIZE 80

int main(int argc, char* argv[])
{
	FILE* img = NULL;
	int nerrors;
	bstring pp_result_name;
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
		arg_print_syntax(stderr, argtable, "\n");
		fprintf(stderr,"options:\n");
		arg_print_glossary(stderr, argtable, "    %-25s %s\n");
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
	ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
	pp_result_name = pp_do(bautofree(bfromcstr(input_file->filename[0])));
	if (pp_result_name == NULL)
	{
		fprintf(stderr, "assembler: invalid result returned from preprocessor.\n");
		pp_cleanup(bautofree(pp_result_name));
		return 1;
	}
	yyout = stderr;
	yyin = fopen((const char*)(pp_result_name->data), "r");
	if (yyin == NULL)
	{
		pp_cleanup(bautofree(pp_result_name));
		return 1;
	}

	// Parse assembly.
	yyparse();
	if (yyin != stdin)
		fclose(yyin);
	pp_cleanup(bautofree(pp_result_name));
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
		osutil_makebinary(stdout);

		// Set img to stdout.
		img = stdout;
	}
	
	// Write content.
	aout_write(img, (gen_relocatable->count > 0), (gen_intermediate->count > 0));
	fclose(img);
	fprintf(stderr, "assembler: completed successfully.\n");
	
	return 0;
}
