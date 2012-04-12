/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argtable2.h>
#include "dcpu.h"

int main(int argc, char* argv[])
{
	// Define our variables.
	FILE* load;
	uint16_t flash[0x10000];
	unsigned int i, a;
	int cread;
	bool uread = true;
	vm_t* vm;
	int nerrors;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file, or - to read from standard input.");
	struct arg_lit* debug_mode = arg_lit0("d", "debug", "Show each executed instruction.");
	struct arg_end *end = arg_end(20);
	void *argtable[] = { input_file, debug_mode, end };
	
	// Parse arguments.
	nerrors = arg_parse(argc,argv,argtable);
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "emulator");
		printf("syntax:\n    emulator");
		arg_print_syntax(stdout, argtable, "\n");
		printf("options:\n");
		arg_print_glossary(stdout, argtable, "    %-25s %s\n");
		exit(1);
	}

	// Zero out the flash space.
	for (i = 0; i < 0x10000; i++)
		flash[i] = 0x0;
	
	// Load from either file or stdin.
	if (strcmp(input_file->filename[0], "-") != 0)
	{
		// Open file.
		load = fopen(input_file->filename[0], "rb");
		if (load == NULL)
		{
			printf("Unable to load %s from disk.\n", argv[1]);
			exit(1);
		}
	}
	else
	{
		// Windows needs stdin in binary mode.
#ifdef _WIN32
		_setmode( _fileno( stdin ), _O_BINARY );
#endif

		// Set load to stdin.
		load = stdin;
	}

	// Read up to 0x20000 bytes (as 16-bit words).
	a = 0;
	for (i = 0; i < 0x20000; i++)
	{
		cread = fgetc(load);
		if (cread == -1) break;
		if (uread)
			cread <<= 8;
		flash[a] += ((cread << 8) | (cread >> 8));
		if (!uread)
			a += 1;
		uread = !uread;
	}
	fclose(load);

	// And then use the VM.
	vm = vm_create();
	vm->debug = (debug_mode->count > 0);
	vm_flash(vm, flash);
	vm_execute(vm);
	vm_free(vm);

	return 0;
}