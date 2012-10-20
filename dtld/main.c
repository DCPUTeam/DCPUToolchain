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
#include <iio.h>
#include <derr.h>
#include "objfile.h"
#include "lprov.h"
#include "ldata.h"
#include "ldbins.h"
#include "ldtarget.h"

int main(int argc, char* argv[])
{
	// Define our variables.
	int nerrors, i;
	int32_t saved = 0; // The number of words saved during compression and optimization.
	struct errinfo* errval;
	const char* prepend = "error: ";
	const char* warnprefix = "no-";
	int msglen;
	char* msg;
    int target;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* target_arg = arg_str0("l", "link-as", "target", "Link as the specified object, can be 'image', 'static' or 'kernel'.");
	struct arg_file* symbol_file = arg_file0("s", "symbols", "<file>", "Produce a combined symbol file (~triples memory usage!).");
	struct arg_str* symbol_ext = arg_str0(NULL, "symbol-extension", "ext", "When -s is used, specifies the extension for symbol files.  Defaults to \"dsym16\".");
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
    struct arg_file* kernel_file = arg_file0("k", "kernel", "<file>", "Directly link in the specified kernel.");
    struct arg_file* jumplist_file = arg_file0("j", "jumplist", "<file>", "Link against the specified jumplist.");
	struct arg_str* warning_policies = arg_strn("W", NULL, "policy", 0, _WARN_COUNT * 2 + 10, "Modify warning policies.");
	struct arg_lit* keep_output_arg = arg_lit0(NULL, "keep-outputs", "Keep the .OUTPUT entries in the final static library (used for stdlib).");
	struct arg_lit* little_endian_mode = arg_lit0(NULL, "little-endian", "Use little endian serialization (for compatibility with older versions).");
	struct arg_lit* no_short_literals_arg = arg_lit0(NULL, "no-short-literals", "Do not compress literals to short literals.");
	struct arg_int* opt_level = arg_int0("O", NULL, "<level>", "The optimization level.");
	struct arg_lit* opt_mode = arg_lit0("S", NULL, "Favour runtime speed over size when optimizing.");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, target_arg, keep_output_arg, little_endian_mode, opt_level, opt_mode, no_short_literals_arg,
	symbol_ext, symbol_file, kernel_file, jumplist_file, warning_policies, output_file, input_files, verbose, quiet, end };

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

	// Set endianness.
	isetmode(little_endian_mode->count == 0 ? IMODE_BIG : IMODE_LITTLE);

    // Set up warning policies.
    dsetwarnpolicy(warning_policies);

	// Set up error handling.
	errval = (struct errinfo*)dsethalt();
	if (errval != NULL)
	{
		// FIXME: Use bstrings here.
		msglen = strlen(derrstr[errval->errid]) + strlen(prepend) + 1;
		msg = malloc(msglen);
		memset(msg, '\0', msglen);
		strcat(msg, prepend);
		strcat(msg, derrstr[errval->errid]);
		printd(LEVEL_ERROR, msg, errval->errdata);

		// Handle the error.
		printd(LEVEL_ERROR, "linker: error occurred.\n");

		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Check to make sure target is correct.
	if (target_arg->count == 0)
		target = IMAGE_APPLICATION;
	else
	{
		if (strcmp(target_arg->sval[0], "image") == 0)
			target = IMAGE_APPLICATION;
		else if (strcmp(target_arg->sval[0], "static") == 0)
			target = IMAGE_STATIC_LIBRARY;
		else if (strcmp(target_arg->sval[0], "kernel") == 0)
			target = IMAGE_KERNEL;
	else
		{
			// Invalid option.
	    dhalt(ERR_INVALID_TARGET_NAME, NULL);
		}
	}

	// Load all passed objects and use linker bin system to
	// produce result.
	bins_init();
	for (i = 0; i < input_files->count; i++)
		if (!bins_load(bautofree(bfromcstr(input_files->filename[i])), symbol_file->count > 0, (symbol_file->count > 0 && symbol_ext->count > 0) ? symbol_ext->sval[0] : "dsym16"))
			// Failed to load one of the input files.
	    dhalt(ERR_BIN_LOAD_FAILED, input_files->filename[i]);
	bins_associate();
	bins_sectionize();
	bins_flatten(bautofree(bfromcstr("output")));
    if (target == IMAGE_KERNEL)
	bins_write_jump();
	saved = bins_optimize(
			opt_mode->count == 0 ? OPTIMIZE_SIZE : OPTIMIZE_SPEED,
			opt_level->count == 0 ? OPTIMIZE_NONE : opt_level->ival[0]);
	if (no_short_literals_arg->count == 0 && target != IMAGE_STATIC_LIBRARY)
		saved += bins_compress();
	else if (no_short_literals_arg->count == 0)
	dwarn(WARN_SKIPPING_SHORT_LITERALS_TYPE, NULL);
	else
	dwarn(WARN_SKIPPING_SHORT_LITERALS_REQUEST, NULL);
	bins_resolve(
	    target == IMAGE_STATIC_LIBRARY,
	    target == IMAGE_STATIC_LIBRARY);
	bins_save(
	    bautofree(bfromcstr("output")),
	    bautofree(bfromcstr(output_file->filename[0])),
	    target,
	    keep_output_arg->count > 0,
	    symbol_file->count > 0 ? symbol_file->filename[0] : NULL,
	    jumplist_file->count > 0 ? jumplist_file->filename[0] : NULL);
	bins_free();
	if (saved > 0)
		printd(LEVEL_DEFAULT, "linker: saved %i words during optimization.\n", saved);
	else if (saved < 0)
		printd(LEVEL_DEFAULT, "linker: increased by %i words during optimization.\n", -saved);

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

