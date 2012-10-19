/**

    File:	    main.c

    Project:	    DCPU-16 Tools
    Component:	    Emulator

    Authors:	    James Rhodes
		    Tyrel Haveman
		    Jose Manuel Diez
		    Patrick Flick

    Description:    Main entry point.

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <libgen.h>
#endif

#include <GL/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bstring.h>
#include <argtable2.h>
#include <dcpu.h>
#include <osutil.h>
#include <version.h>
#include <ldata.h>
#include <debug.h>
#include <iio.h>

// hardware
#include <hwsped3.h>
#include <hwlem1802.h>
#include <hwlem1802mem.h>
#include <hwtimer.h>
#include <hwm35fd.h>
#include <hwlua.h>
#include <hw.h>

int main(int argc, char* argv[])
{
    // Define our variables.
    FILE* load;
    uint16_t flash[0x10000];
    char leading[0x100];
    unsigned int i;
    bool uread = true;
    vm_t* vm;
    int nerrors;
    bstring ss, st;

    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file, or - to read from standard input.");
    struct arg_file* execution_dump_file = arg_file0("e", "execution-dump", "<file>", "Produce a very large execution dump file.");
    struct arg_lit* debug_mode = arg_lit0("d", "debug", "Show each executed instruction.");
    struct arg_lit* terminate_mode = arg_lit0("t", "show-on-terminate", "Show state of machine when program is terminated.");
    struct arg_lit* legacy_mode = arg_lit0("l", "legacy", "Automatically initialize hardware to legacy values.");
    struct arg_lit* little_endian_mode = arg_lit0(NULL, "little-endian", "Use little endian serialization (for compatibility with older versions).");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,	 0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_int* radiation = arg_intn("r", NULL, "<n>", 0, 1, "Radiation factor (higher is less radiation)");
    struct arg_lit* catch_fire = arg_lit0("c", "catch-fire", "The virtual machine should catch fire instead of halting.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { input_file, debug_mode, execution_dump_file, terminate_mode, legacy_mode, little_endian_mode, radiation, catch_fire, verbose, quiet, end };

    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    version_print(bautofree(bfromcstr("Emulator")));
    if (nerrors != 0 || show_help->count != 0)
    {
	if (show_help->count != 0)
	    arg_print_errors(stdout, end, "emulator");

	printd(LEVEL_DEFAULT, "syntax:\n    dtemu");
	arg_print_syntax(stderr, argtable, "\n");
	printd(LEVEL_DEFAULT, "options:\n");
	arg_print_glossary(stderr, argtable, "	    %-25s %s\n");
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 1;
    }

    // Set verbosity level.
    debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Set endianness.
    isetmode(little_endian_mode->count == 0 ? IMODE_BIG : IMODE_LITTLE);

    // Zero out the flash space.
    for (i = 0; i < 0x10000; i++)
	flash[i] = 0x0;

    // Zero out the leading space.
    for (i = 0; i < 0x100; i++)
	leading[i] = 0x0;

    // Load from either file or stdin.
    if (strcmp(input_file->filename[0], "-") != 0)
    {
	// Open file.
	load = fopen(input_file->filename[0], "rb");

	if (load == NULL)
	{
	    fprintf(stderr, "emulator: unable to load %s from disk.\n", argv[1]);
	    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	    return 1;
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

    // Read up to 0x10000 words.
    for (i = 0; i < 0x10000 && !feof(load); i++)
	iread(&flash[i], load);
    fclose(load);

    // Check to see if the first X bytes matches the header
    // for intermediate code and stop if it does.
    ss = bfromcstr("");
    st = bfromcstr(ldata_objfmt);
    for (i = 0; i < strlen(ldata_objfmt); i++)
	bconchar(ss, leading[i]);
    if (biseq(ss, st))
    {
	fprintf(stderr, "emulator: it appears you passed intermediate code for execution.  link\n");
	fprintf(stderr, "	the input code with the toolchain linker to execute it.\n");
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 1;
    }


    glfwInit();
    // And then use the VM.
    vm = vm_create();
    vm->debug = (debug_mode->count > 0);
    vm_flash(vm, flash);

    // Set radiation and catch fire settings.
    if (radiation->count == 1)
	vm->radiation_factor = radiation->ival[0];
    if (catch_fire->count == 1)
	vm->can_fire = true;

    // Init hardware.
    vm_hw_timer_init(vm);
    vm_hw_lem1802_init(vm);
    vm_hw_sped3_init(vm);
    vm_hw_m35fd_init(vm);
    vm_hw_lua_init(vm);

    if (legacy_mode->count > 0)
    {
	for (i = 0; i < vm_hw_count(vm); i++)
	    if (vm_hw_get_device(vm, i).id == 0x7349F615 && vm_hw_get_device(vm, i).manufacturer == 0x1C6C8B36)
	    {
		vm_hw_lem1802_mem_set_screen((struct lem1802_hardware*)vm_hw_get_device(vm, i).userdata, 0x8000);
		break;
	    }
    }
    vm_execute(vm, execution_dump_file->count > 0 ? execution_dump_file->filename[0] : NULL);

    if (terminate_mode->count > 0)
    {
	fprintf(stderr, "\n");
	fprintf(stderr, "A:   0x%04X	 [A]:	 0x%04X\n", vm->registers[REG_A], vm->ram[vm->registers[REG_A]]);
	fprintf(stderr, "B:   0x%04X	 [B]:	 0x%04X\n", vm->registers[REG_B], vm->ram[vm->registers[REG_B]]);
	fprintf(stderr, "C:   0x%04X	 [C]:	 0x%04X\n", vm->registers[REG_C], vm->ram[vm->registers[REG_C]]);
	fprintf(stderr, "X:   0x%04X	 [X]:	 0x%04X\n", vm->registers[REG_X], vm->ram[vm->registers[REG_X]]);
	fprintf(stderr, "Y:   0x%04X	 [Y]:	 0x%04X\n", vm->registers[REG_Y], vm->ram[vm->registers[REG_Y]]);
	fprintf(stderr, "Z:   0x%04X	 [Z]:	 0x%04X\n", vm->registers[REG_Z], vm->ram[vm->registers[REG_Z]]);
	fprintf(stderr, "I:   0x%04X	 [I]:	 0x%04X\n", vm->registers[REG_I], vm->ram[vm->registers[REG_I]]);
	fprintf(stderr, "J:   0x%04X	 [J]:	 0x%04X\n", vm->registers[REG_J], vm->ram[vm->registers[REG_J]]);
	fprintf(stderr, "PC:  0x%04X	 SP:	0x%04X\n", vm->pc, vm->sp);
	fprintf(stderr, "EX:  0x%04X	 IA:	0x%04X\n", vm->ex, vm->ia);
    }

    vm_hw_lua_free(vm);
    vm_hw_free_all(vm);
    vm_free(vm);

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    glfwTerminate();
    return 0;
}
