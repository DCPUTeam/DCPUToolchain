/**

	File:		main.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdlib.h>
#include <argtable2.h>
#include <bstring.h>
#include <version.h>
#include <debug.h>
#include "tests.h"

int main(int argc, char* argv[])
{
	bstring test;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* test_name = arg_str1(NULL, NULL, "<test>", "The name of the test to run.");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, test_name, verbose, quiet, end };

	// Parse arguments.
	int nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("LibDCPU Test Driver")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stderr, end, "ltestdcpu");

		printd(LEVEL_DEFAULT, "syntax:\n    ltestdcpu");
		arg_print_syntax(stderr, argtable, "\n");
		printd(LEVEL_DEFAULT, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Set verbosity level.
	debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

	// Copy the test name and run the appropriate test.
	test = bfromcstr(test_name->sval[0]);
	if (biseqcstr(test, "bopen-01"))
		return bopen_01();
	else if (biseqcstr(test, "bopen-02"))
		return bopen_02();
	else if (biseqcstr(test, "bread-01"))
		return bread_01();
	else if (biseqcstr(test, "bread-02"))
		return bread_02();
	else if (biseqcstr(test, "bcopy-01"))
		return bcopy_01();
	else if (biseqcstr(test, "btell-01"))
		return btell_01();
	else if (biseqcstr(test, "btell-02"))
		return btell_02();

	return 1;
}
