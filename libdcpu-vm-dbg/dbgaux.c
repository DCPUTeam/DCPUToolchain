/**

	File:           dbgaux.c

	Project:        DCPU-16 Toolchain
	Component:      LibDCPU-vm-dbg

	Authors:        James Rhodes
	                José Manuel Díez

	Description:    Defines auxilary functions for the debugger.

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bstrlib.h>
#include <dcpu.h>
#include <dcpuhook.h>
#include <hwio.h>
#include <lem1802.h>
#include <hwtimer.h>

#define MAX_BREAKPOINTS 100

uint16_t flash[0x10000];
uint16_t breakpoints[MAX_BREAKPOINTS];
uint16_t breakpoints_num;
extern vm_t* vm;

void ddbg_cycle_hook(vm_t* vm, uint16_t pos)
{
	int i = 0;	
	for(i = 0; i < breakpoints_num; i++)
	{
		if(vm->pc == breakpoints[i] && vm->pc != 0xFFFF)
		{
			vm->halted = true;
			printf("Breakpoint hit at 0x%04X.\n", breakpoints[i]);
		}
	}
}

void ddbg_load(bstring path)
{
	FILE* load;
	unsigned int a = 0, i = 0;
	int cread;
	bool uread = true;
	
	load = fopen(path->data, "rb");
	if (load == NULL)
	{
		printf("Unable to load %s from disk.\n", path->data);
		exit(1);
	}

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
	
	printf("Loaded 0x%04X words from %s.\n", a, path->data);
}

void ddbg_create_vm()
{
	vm = vm_create();
	vm_hook_register(vm, &ddbg_cycle_hook, HOOK_ON_CYCLE);
	printf("Created VM.\n");
}

void ddbg_flash_vm()
{
	vm_flash(vm, flash);
	printf("Flashed memory.\n");
}

void ddbg_run_vm()
{
	vm->halted = false;
	vm_execute(vm);
	printf("\n");
}

void ddbg_attach(bstring hw)
{
	if (biseq(hw, bfromcstr("lem1802")))
		vm_lem1802_init(vm, 0);
	else if (biseq(hw, bfromcstr("keyboard")))
		vm_hw_io_init(vm, 0);
	else if (biseq(hw, bfromcstr("clock")))
		vm_hw_timer_init(vm);
	else
		printf("Unrecognized hardware.\n");
}

void ddbg_add_breakpoint(bstring file, int index)
{
	if (!biseq(file, bfromcstr("memory")))
	{
		printf("Only memory breakpoints can be set at this time (use memory:address).\n");
		return;
	}
	
	if (index < 0)
	{
		printf("Index must be greater than 0.\n");
		return;
	}
	
	breakpoints[breakpoints_num++] = index;
	printf("Breakpoint added at 0x%04X.\n", index);
}

void ddbg_delete_breakpoint(bstring file, int index) 
{
	int i = 0;
	bool found = false;

	if (!biseq(file, bfromcstr("memory")))
	{
		printf("Only memory breakpoints can be set at this time (use memory:address).\n");
		return;
	}
	
	if (index < 0)
	{
		printf("Index must be greater than 0.\n");
		return;
	}
	
	for(i = 0; i < breakpoints_num; i++)
	{
		if(breakpoints[i] == index)
		{
			breakpoints[i] = 0xFFFF;
			found = true;
		}
	}
	
	if(found == true) {
		printf("Breakpoint at %s:%d removed.\n", bstr2cstr(file, 0), index);
	} else {
		printf("There was no breakpoint at %s:%d.\n", bstr2cstr(file, 0), index);
	}
}

void ddbg_breakpoints_list()
{
	int i = 0;
	
	printf("%d breakpoints loaded.\n", breakpoints_num);
	for(i = 0; i < breakpoints_num; i++) printf("- at 0x%04X\n", breakpoints[i]);
}

void ddbg_dump_state()
{
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



void ddbg_dump_ram(int start, int difference)
{
	int end;
	int i = 0;
	
	if (start < 0 || difference < 0)
	{
		printf("Invalid parameters provided to 'inspect memory'.");
		return;
	}
	
	if (difference == 0)
		end = start + 32;
	else
		end = start + difference;
	
	for(i = 0; i < difference; i++)
	{
		if (i % 8 == 0)
			printf("%04X: ", start + i);
		printf("%04X ", vm->ram[start + i]);
		if ((i + 1) % 8 == 0)
			printf("\n");
	}
	
	printf("\n");
}
