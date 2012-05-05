/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Emulator

	Authors:	James Rhodes
			Tyrel Haveman

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
#include <bstring.h>
#include <argtable2.h>
#include "dcpu.h"
#include <hwio.h>
#include <hwtimer.h>
#include <hwlem1802.h>
#include <hwlem1802mem.h>
#include <osutil.h>

#ifdef __APPLE__
	#define main SDL_main
#endif

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
	struct arg_lit* terminate_mode = arg_lit0("t", "show-on-terminate", "Show state of machine when program is terminated.");
	struct arg_lit* legacy_mode = arg_lit0("l", "legacy", "Automatically initialize hardware to legacy values.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { input_file, debug_mode, terminate_mode, legacy_mode, end };

	// Parse arguments.
	nerrors = arg_parse(argc, argv, argtable);

	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "emulator");

		printf("syntax:\n    emulator");
		arg_print_syntax(stderr, argtable, "\n");
		printf("options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		exit(1);
	}

	// Set global path variable.
	osutil_setarg0(bautofree(bfromcstr(argv[0])));

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
		_setmode(_fileno(stdin), _O_BINARY);
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
	vm_hw_lem1802_init(vm);
	vm_hw_io_init(vm);
	vm_hw_timer_init(vm);
	if (legacy_mode->count > 0)
	{
		vm_hw_lem1802_mem_set_screen(vm, 0x8000);
		vm_hw_io_set_legacy(true);
	}
	vm_execute(vm);

	if (terminate_mode->count > 0)
	{
		fprintf(stderr, "\n");
		fprintf(stderr, "A:   0x%04X	 [A]:	0x%04X\n", vm->registers[REG_A], vm->ram[vm->registers[REG_A]]);
		fprintf(stderr, "B:   0x%04X	 [B]:	0x%04X\n", vm->registers[REG_B], vm->ram[vm->registers[REG_B]]);
		fprintf(stderr, "C:   0x%04X	 [C]:	0x%04X\n", vm->registers[REG_C], vm->ram[vm->registers[REG_C]]);
		fprintf(stderr, "X:   0x%04X	 [X]:	0x%04X\n", vm->registers[REG_X], vm->ram[vm->registers[REG_X]]);
		fprintf(stderr, "Y:   0x%04X	 [Y]:	0x%04X\n", vm->registers[REG_Y], vm->ram[vm->registers[REG_Y]]);
		fprintf(stderr, "Z:   0x%04X	 [Z]:	0x%04X\n", vm->registers[REG_Z], vm->ram[vm->registers[REG_Z]]);
		fprintf(stderr, "I:   0x%04X	 [I]:	0x%04X\n", vm->registers[REG_I], vm->ram[vm->registers[REG_I]]);
		fprintf(stderr, "J:   0x%04X	 [J]:	0x%04X\n", vm->registers[REG_J], vm->ram[vm->registers[REG_J]]);
		fprintf(stderr, "PC:  0x%04X	 SP:	0x%04X\n", vm->pc, vm->sp);
		fprintf(stderr, "EX:  0x%04X	 IA:	0x%04X\n", vm->ex, vm->ia);
	}

	vm_free(vm);

	return 0;
}
