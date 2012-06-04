/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Virtual Machine Test Suite

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdlib.h>
#include <stdio.h>
#include <argtable2.h>
#include <bstring.h>
#include <bfile.h>
#include <version.h>
#include <debug.h>
#include <osutil.h>

int main(int argc, char* argv[])
{
	bstring cmdargs = bfromcstr("");
	int i, result;
	unsigned int match = 0, unmatch = 0;
	char ca, ce;
	char* file_cpu;
	char* file_sym;
#ifndef _WIN32
	char* mod_path;
	char* temp;
#endif

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input assembly file containing unit tests.");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, input_file, verbose, quiet, end };

	// Parse arguments.
	int nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Virtual Machine Unit Test Driver")));
	if (nerrors != 0 || show_help->count != 0)
	{
		printd(LEVEL_DEFAULT, "syntax:\n    testvm");
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
	
	// Get temporary names for each intermediate file.
	file_cpu = tempnam(NULL, "vtcpu");
	file_sym = tempnam(NULL, "vtsym");
	
#ifndef _WIN32
	// We need to get the existing TOOLCHAIN_MODULES
	// environment variable so we can set it back later
	// on.
	mod_path = getenv("TOOLCHAIN_MODULES");
	bassigncstr(cmdargs, argv[0]);
	btrunc(cmdargs, binstr(cmdargs, 0, bfromcstr("testvm")));
	bcatcstr(cmdargs, "../tests/drivers/testvm/modules"); // Totally not portable!
	printd(LEVEL_DEFAULT, "%s\n", cmdargs->data);
	setenv("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'), 1);
	bcstrfree(temp);
#else
	printd(LEVEL_WARNING, "warning: Unable to set TOOLCHAIN_MODULES temporarily on Windows systems for\n");
	printd(LEVEL_WARNING, "         testing purposes.  Install units, unitsdbg, assert and assertdbg\n");
	printd(LEVEL_WARNING, "         globally to ensure that the test suite runs correctly.\n");
#endif

	// Generate the argument list for the assembler.
	bassigncstr(cmdargs, argv[0]);
	bfindreplace(cmdargs, bfromcstr("testvm"), bfromcstr("dtasm"), 0);
	binsertch(cmdargs, 0, 1, '"');
	bconchar(cmdargs, '"');
	bconchar(cmdargs, ' ');
	
	// Symbols file.
	bcatcstr(cmdargs, "-s \"");
	bcatcstr(cmdargs, file_sym);
	bcatcstr(cmdargs, "\" ");

	// Output file.
	bcatcstr(cmdargs, "-o \"");
	bcatcstr(cmdargs, file_cpu);
	bcatcstr(cmdargs, "\" ");

	// Input file.
	bcatcstr(cmdargs, "\"");
	bcatcstr(cmdargs, input_file->filename[0]);
	bcatcstr(cmdargs, "\" ");

	// Windows needs the whole command wrapped in quotes and slashes to be correct.
	// See http://stackoverflow.com/questions/2642551/windows-c-system-call-with-spaces-in-command.
#ifdef _WIN32
	binsertch(cmdargs, 0, 1, '"');
	bconchar(cmdargs, '"');
#endif

	// Now run the assembler!
	result = system(cmdargs->data);
	if (result != 0)
	{
		// Assembler returned error exit code.
		printd(LEVEL_ERROR, "error: unable to assemble unit test (assembler returned non-zero exit code '%i').\n", result >> 8);
		return 1;
	}
	
	// We now need to set up the command to
	// run the debugger to invoke the unit tests.
	bassigncstr(cmdargs, argv[0]);
	bfindreplace(cmdargs, bfromcstr("testvm"), bfromcstr("dtdb"), 0);
	binsertch(cmdargs, 0, 1, '"');
	bconchar(cmdargs, '"');
	bconchar(cmdargs, ' ');
	
	// Run the 'test' command automatically.
	bcatcstr(cmdargs, "-c test ");
	
	// Symbols file.
	bcatcstr(cmdargs, "-s \"");
	bcatcstr(cmdargs, file_sym);
	bcatcstr(cmdargs, "\" ");
	
	// Input file.
	bcatcstr(cmdargs, "\"");
	bcatcstr(cmdargs, file_cpu);
	bcatcstr(cmdargs, "\" ");
	
	// Now run the debugger!
	result = system(cmdargs->data);
	if (result != 0)
	{
		// Unit test failed!
		unlink(file_cpu);
		unlink(file_sym);
		
#ifndef _WIN32
		// Restore TOOLCHAIN_MODULES.
		if (mod_path == NULL)
			unsetenv("TOOLCHAIN_MODULES");
		else
		{
			bassigncstr(cmdargs, mod_path);
			setenv("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'), 1);
			bcstrfree(temp);
		}
#endif
		
		// Show error message (is 'result >> 8' modification only applicable to UNIX)?
		printd(LEVEL_ERROR, "error: unit test failed with exit code %i (see above output).\n", result >> 8);
		return 1;
	}
	
	// Delete temporary files.
	unlink(file_cpu);
	unlink(file_sym);

#ifndef _WIN32
	// Restore TOOLCHAIN_MODULES.
	if (mod_path == NULL)
		unsetenv("TOOLCHAIN_MODULES");
	else
	{
		bassigncstr(cmdargs, mod_path);
		setenv("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'), 1);
		bcstrfree(temp);
	}
#endif

	return 0;
}
