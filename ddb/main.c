/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

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

#include <dcpu.h>
#include <dcpuhook.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

#include <signal.h>

#include "sdp.h"

#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 50
#define MAX_BREAKPOINTS 100

char* path;
uint16_t flash[0x10000];
uint16_t breakpoints[MAX_BREAKPOINTS];
uint16_t breakpoints_num;
vm_t* vm;
pthread_t sdp_thread;

#ifdef _WIN32
char* dirname(char* fixpath)
{
	// FIXME: This assumes the resulting path will always
	// be shorter than the original (which it should be
	// given that we're only returning a component of it, right?)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(fixpath, drive, dir, NULL, NULL);
	strcpy(fixpath, drive);
	strcpy(fixpath + strlen(fixpath), dir);
	return fixpath;
}
#endif

void ddbg_sigint(int signal) {
	pthread_cancel(sdp_thread);
	exit(1);
}

void ddbg_cycle_hook(vm_t* vm, uint16_t pos)
{
	int i = 0;
	
	for(i = 0; i < breakpoints_num; i++) {
		if(vm->pc == breakpoints[i]) {
			vm->halted = true;
			printf("Breakpoint hit at %04X.\n", breakpoints[i]);
		}
	}
} 

void get_command(char* command_buffer, int max) {
	printf("> ");
	fgets(command_buffer, max, stdin);
}	

void ddbg_load(int argc, char** argv) {
	FILE* load;
	unsigned int a = 0, i = 0;
	int cread;
	bool uread = true;
	
	load = fopen(argv[1], "rb");
	if (load == NULL) {
		printf("Unable to load %s from disk.\n", argv[1]);
		exit(1);
	}

	for (i = 0; i < 0x20000; i++) {
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
	
	printf("Loaded 0x%04X words from %s.\n", a, argv[1]);
}

void ddbg_set_debug_vm(int argc, char** argv) {
	vm->debug = atoi(argv[1]);
}

void ddbg_create_vm() {
	vm = vm_create();
	vm_hook_register(vm, &ddbg_cycle_hook, HOOK_ON_CYCLE);
	printf("Created VM.\n");
}

void ddbg_flash_vm() {
	vm_flash(vm, flash);
	printf("Flashed memory.\n");
}

void ddbg_run_vm() {
	vm_execute(vm);
}

void ddbg_attach(int argc, char** argv) {
	if(strcmp(argv[1], "lem1802") == 0) {
		vm_lem1802_init(vm, 0);
	} else if(strcmp(argv[1], "generic_keyboard") == 0) {
		vm_hw_io_init(vm, 0);
	} else if(strcmp(argv[1], "generic_clock") == 0 ) {
		vm_hw_timer_init(vm);
	} else {
		printf("Unrecognized hardware.\n");
	}
}

void ddbg_quickstart(int argc, char** argv) {
	ddbg_load(argc, argv);
	ddbg_create_vm();
	ddbg_flash_vm();
}

void ddbg_add_breakpoint(int argc, char** argv) {
	uint16_t address;
	
	address = atoi(argv[1]);
	
	breakpoints[breakpoints_num++] = address;
	printf("Breakpoint added at 0x%04X\n", address);
}

void ddbg_breakpoints_list() {
	int i = 0;
	
	printf("%d breakpoints loaded.\n", breakpoints_num);
	for(i = 0; i < breakpoints_num; i++) printf("- at 0x%04X\n", breakpoints[i]);
}

void ddbg_dump_state() {
	fprintf(stderr, "A:   0x%04X     [A]:   0x%04X\n", vm->registers[REG_A], vm->ram[vm->registers[REG_A]]);
	fprintf(stderr, "B:   0x%04X     [B]:   0x%04X\n", vm->registers[REG_B], vm->ram[vm->registers[REG_B]]);
	fprintf(stderr, "C:   0x%04X     [C]:   0x%04X\n", vm->registers[REG_C], vm->ram[vm->registers[REG_C]]);
	fprintf(stderr, "X:   0x%04X     [X]:   0x%04X\n", vm->registers[REG_X], vm->ram[vm->registers[REG_X]]);
	fprintf(stderr, "Y:   0x%04X     [Y]:   0x%04X\n", vm->registers[REG_Y], vm->ram[vm->registers[REG_Y]]);
	fprintf(stderr, "Z:   0x%04X     [Z]:   0x%04X\n", vm->registers[REG_Z], vm->ram[vm->registers[REG_Z]]);
	fprintf(stderr, "I:   0x%04X     [I]:   0x%04X\n", vm->registers[REG_I], vm->ram[vm->registers[REG_I]]);
	fprintf(stderr, "J:   0x%04X     [J]:   0x%04X\n", vm->registers[REG_J], vm->ram[vm->registers[REG_J]]);
	fprintf(stderr, "PC:  0x%04X     SP:    0x%04X\n", vm->pc, vm->sp);
	fprintf(stderr, "EX:  0x%04X     IA:    0x%04X\n", vm->ex, vm->ia);
}

void ddbg_dump_ram(int argc, char** argv) {
	uint16_t start, end, difference;
	uint16_t i = 0;
	
	start = atoi(argv[1]);
	if(argc == 2) {
		end = start;
	} else end = atoi(argv[2]);
	difference = end - start;
	
	for(i = 0; i <= difference; i++) {
		if(i % 8 == 0) printf("\n%04X: ", start+i);
		printf("%04X ", vm->ram[start + i]);
		
	} 
	
	printf("\n");
}

int main(int argc, char** argv) {
	char* buf;
	char** args;
	char* tmp;
	unsigned int num_args = 0;
	unsigned int i = 0;
	int child = 0;
	
	
	args = malloc(sizeof(char*) * MAX_ARGUMENTS);
	for(i = 0; i < MAX_ARGUMENTS; i++) args[i] = malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
	
	// Set global path variable.
	path = strdup(argv[0]);
	path = (char*) dirname(path);
	
	signal(SIGINT, ddbg_sigint);
	
	pthread_create(&sdp_thread, NULL, (void*)ddbg_sdp_thread, vm);
	printf("Welcome to the DCPU Toolchain Debugger, the best debugger in the multiverse.\n");
	
	for(;;) {
		buf = readline("> ");
		add_history (buf);
		
		if(strrchr(buf, ' ')) {
			num_args = 0;
			i = 0;
			
			tmp = strtok(buf, " ");
			while(tmp != NULL) {
				strcpy(args[i], tmp);
				
				i++;
				tmp = strtok(NULL, " ");
			}
			
			num_args = i;
			
			if(strcmp(args[0], "load") == 0) {
				ddbg_load(num_args, args);
			} else if(strcmp(args[0], "set_debug") == 0) {
				ddbg_set_debug_vm(num_args, args);
			} else if(strcmp(args[0], "attach") == 0) {
				ddbg_attach(num_args, args);
			} else if(strcmp(args[0], "quickstart") == 0) {
				ddbg_quickstart(num_args, args);
			} else if(strcmp(args[0], "break") == 0) {
				ddbg_add_breakpoint(num_args, args);
			} else if(strcmp(args[0], "dump_ram") == 0) {
				ddbg_dump_ram(num_args, args);
			}
		} else {
			if(strcmp(buf, "exit") == 0) {
				exit(1);
			} else if(strcmp(buf, "flash") == 0) {
				ddbg_flash_vm();
			} else if(strcmp(buf, "create_vm") == 0) {
				ddbg_create_vm();
			} else if(strcmp(buf, "run") == 0) {
				ddbg_run_vm();
			} else if(strcmp(buf, "break_list") == 0) {
				ddbg_breakpoints_list();
			} else if(strcmp(buf, "state") == 0) {
				ddbg_dump_state();
			} 
		}
	}
	
    free(buf);
	
	return 0;
}
