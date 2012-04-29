#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dcpu.h>
#include <dcpuhook.h>

#define MAX_BREAKPOINTS 100

uint16_t flash[0x10000];
uint16_t breakpoints[MAX_BREAKPOINTS];
uint16_t breakpoints_num;
vm_t* vm;

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

void ddbg_load(char* path) {
	FILE* load;
	unsigned int a = 0, i = 0;
	int cread;
	bool uread = true;
	
	load = fopen(path, "rb");
	if (load == NULL) {
		printf("Unable to load %s from disk.\n", path);
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
	
	printf("Loaded 0x%04X words from %s.\n", a, path);
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

void ddbg_quickstart(char* path) {
	ddbg_load(path);
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

