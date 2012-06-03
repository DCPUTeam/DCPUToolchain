/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Linker

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <argtable2.h>
#include <version.h>
#include <debug.h>
#include <osutil.h>
#include "objfile.h"
#include "lprov.h"
#include "ldata.h"
#include "ldbins.h"

int main(int argc, char* argv[])
{
	// Define our variables.
	int nerrors, i;
	bstring target;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* target_arg = arg_str0("l", "link-as", "target", "Link as the specified object, can be 'image' or 'static'.");
	struct arg_file* symbol_file = arg_file0("s", "symbols", "<file>", "Produce a combined symbol file (~triples memory usage!).");
	struct arg_str* symbol_ext = arg_str0(NULL, "symbol-extension", "ext", "When -s is used, specifies the extension for symbol files.  Defaults to \"dsym16\".");
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_lit* keep_output_arg = arg_lit0(NULL, "keep-outputs", "Keep the .OUTPUT entries in the final static library (used for stdlib).");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, target_arg, keep_output_arg, symbol_ext, symbol_file, output_file, input_files, verbose, quiet, end };

	// Parse arguments.
	nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Linker")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "linker");

		printd(LEVEL_DEFAULT, "syntax:\n    dtld");
		arg_print_syntax(stderr, argtable, "\n");
		printd(LEVEL_DEFAULT, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Set verbosity level.
	debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

	// Set global path variable.
	osutil_setarg0(bautofree(bfromcstr(argv[0])));

	// Check to make sure target is correct.
	if (target_arg->count == 0)
		target = bfromcstr("image");
	else
	{
		if (strcmp(target_arg->sval[0], "image") == 0)
			target = bfromcstr("image");
		else if (strcmp(target_arg->sval[0], "static") == 0)
			target = bfromcstr("static");
		else
		{
			// Invalid option.
			printd(LEVEL_ERROR, "linker: invalid target type, must be 'image' or 'static'.\n");
			arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
			return 1;
		}
	}

	// Load all passed objects and use linker bin system to
	// produce result.
	bins_init();
	for (i = 0; i < input_files->count; i++)
		bins_load(bautofree(bfromcstr(input_files->filename[i])), symbol_file->count > 0, (symbol_file->count > 0 && symbol_ext->count > 0) ? symbol_ext->sval[0] : "dsym16");
	bins_associate();
	bins_sectionize();
	bins_flatten(bautofree(bfromcstr("output")));
	// TODO: This is where we would perform short literal optimizations
	//	 with bins_compress(); when it's implemented.
	bins_resolve(biseqcstr(target, "static") == true);
	bins_save(bautofree(bfromcstr("output")), bautofree(bfromcstr(output_file->filename[0])), bautofree(target), keep_output_arg->count > 0, symbol_file->count > 0 ? symbol_file->filename[0] : NULL);
	bins_free();
	
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

