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
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, target_arg, input_files, output_file, end };

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
		bins_load(bautofree(bfromcstr(input_files->filename[i])));
	bins_sectionize();
	bins_flatten(bautofree(bfromcstr("output")));
	// TODO: This is where we would perform short literal optimizations
	//	 with bins_compress(); when it's implemented.
	bins_resolve(biseqcstr(target, "static") == true);
	bins_save(bautofree(bfromcstr("output")), bautofree(bfromcstr(output_file->filename[0])));

	return 0;
}

