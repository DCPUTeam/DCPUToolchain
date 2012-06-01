/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Preprocessor

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdio.h>
#include <bstring.h>
#include <simclist.h>
#include <parser.h>
#include <lexer.h>
#include <assert.h>
#include <ppfind.h>
#include <version.h>
#include <argtable2.h>
#include <debug.h>
#include <osutil.h>

extern int pp_yyparse(void* scanner);
extern void handle_start(freed_bstring name, FILE* output);

int main(int argc, char* argv[])
{
	yyscan_t scanner;
	FILE* file;
	bstring path;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file (or - to read from standard input).");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, input_file, verbose, quiet, end };

	// Parse arguments.
	int nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Preprocessor")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stderr, end, "preprocessor");

		fprintf(stderr, "syntax:\n    dtpp");
		arg_print_syntax(stderr, argtable, "\n");
		fprintf(stderr, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Set verbosity level.
	debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

	// Set global path variable.
	osutil_setarg0(bautofree(bfromcstr(argv[0])));

	// Store the path of the input file.
	path = bfromcstr(input_file->filename[0]);

	// Load file or stdin.
	if (biseq(path, bfromcstr("-")))
	{
		ppfind_add_autopath(bautofree(bfromcstr(".")));
		file = stdin;
	}
	else
	{
		ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
		file = fopen(argv[1], "r");
		assert(file != NULL);
	}

	pp_yylex_init(&scanner);
	pp_yyset_out(stdout, scanner);
	pp_yyset_in(file, scanner);
	handle_start(bautofree(bfromcstr(argv[1])), stdout);
	pp_yyparse(scanner);
	pp_yylex_destroy(scanner);
	if (!biseq(path, bfromcstr("-")))
		fclose(file);
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}
