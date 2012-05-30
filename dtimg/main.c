/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Image Smasher

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <argtable2.h>
#include <version.h>
#include <debug.h>
#include "base.h"
#include "micro.h"

#define ENSURE_OPEN(in, name) if (in == NULL) { printd(LEVEL_ERROR, "failed to open %s.\n", name); return 1; }

int main(int argc, char* argv[])
{
	// Define our variables.
	int nerrors, i;
	struct fs_writer* writer;
	FILE* in;
	FILE* out;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* fs_arg = arg_str0("t", "type", "fs", "The filesystem type, currently only supports 'micro'.");
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 0, 100, "The files to store in the disk image.");
	struct arg_file* kernel_file = arg_file1("k", "kernel", "<file>", "The kernel binary to use.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
	struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, fs_arg, kernel_file, output_file, verbose, quiet, input_files, end };

	// Parse arguments.
	nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Image Smasher")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "img");

		printd(LEVEL_DEFAULT, "syntax:\n    dtimg");
		arg_print_syntax(stderr, argtable, "\n");
		printd(LEVEL_DEFAULT, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Set verbosity level.
	debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

	// Check to make sure target is correct.
	if (fs_arg->count == 0)
		writer = writer_get_micro();
	else
	{
		if (strcmp(fs_arg->sval[0], "micro") == 0)
			writer = writer_get_micro();
		else
		{
			// Invalid option.
			printd(LEVEL_ERROR, "linker: invalid filesystem type, must be 'micro'.\n");
			arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
			return 1;
		}
	}

	// Open the file.
	out = fopen(output_file->filename[0], "wb+");
	ENSURE_OPEN(out, output_file->filename[0]);

	// Use the filesystem writer to write out the content.
	writer->init(out);

	// Write out the kernel.
	in = fopen(kernel_file->filename[0], "rb");
	ENSURE_OPEN(in, kernel_file->filename[0]);
	writer->write_kernel(out, in, input_files->count);
	fclose(in);

	// Write out each of the files.
	for (i = 0; i < input_files->count; i += 1)
	{
		in = fopen(input_files->filename[i], "rb");
		ENSURE_OPEN(in, input_files->filename[i]);
		writer->write_file(out, in, bautofree(bfromcstr(input_files->basename[i])));
		fclose(in);
	}

	// Finish writing the filesystem.
	writer->close(out);

	printd(LEVEL_VERBOSE, "finished writing filesystem image.\n");

	return 0;
}
