/**

	File:			dcpu.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Handles high-level operations performed
					on the virtual machine (such as creation).

**/

#include <stdlib.h>
#include <varargs.h>
#include <stdio.h>
#include <stdint.h>
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuscrn.h"

vm_t* vm_create()
{
	// Define variables.
	vm_t* new_vm;
	unsigned int i;

	// Allocate and wipe vm memory.
	new_vm = (vm_t*)malloc(sizeof(vm_t));
	for (i = 0; i < 0x8; i++)
		new_vm->registers[i] = 0x0;
	new_vm->pc = 0x0;
	new_vm->sp = 0x0;
	new_vm->o = 0x0;
	for (i = 0; i < 0x10000; i++)
		new_vm->ram[i] = 0x0;
	new_vm->dummy = 0x0;
	new_vm->halted = false;
	new_vm->skip = false;
	
	// Initialize DCPU-16 components.
	vm_scrn_init(new_vm);

	// Return.
	return new_vm;
}

void vm_free(vm_t* vm)
{
	// Shutdown components.
	vm_scrn_free(vm);

	// Free the memory.
	free(vm);
}

void vm_flash(vm_t* vm, uint16_t memory[0x10000])
{
	// Flash the VM's memory from the specified array.
	unsigned int i;
	for (i = 0; i < 0x8; i++)
		vm->registers[i] = 0x0;
	vm->pc = 0x0;
	vm->sp = 0x0;
	vm->o = 0x0;
	for (i = 0; i < 0x10000; i++)
		vm->ram[i] = memory[i];
	vm->dummy = 0x0;
	vm->halted = false;
	vm->skip = false;
}

void vm_execute(vm_t* vm)
{
	// Execute the memory using DCPU-16 specifications.
	while (!vm->halted)
	{
		vm_cycle(vm);
		vm_scrn_cycle(vm);
	}
}
