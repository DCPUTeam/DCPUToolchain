/**

	File:			hw.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		José Manuel Díez

	Description:	Handles opcode instructions in the
					virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdlib.h>
#include "dcpubase.h"
#include "hw.h"

#define HW_MAX 10

int vm_hw_connected[HW_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
hw_t vm_hw_list[HW_MAX];

uint16_t vm_hw_register(vm_t* vm, hw_t hardware)
{
	uint16_t id = 0;
	while (vm_hw_connected[id] != 0 && id < HW_MAX)
		id++;
	if (id >= HW_MAX)
	{
		vm_halt(vm, "unable to register hardware, maximum reached!");
		return;
	}

	printf("%04X %04X\n", hardware.id_1, hardware.id_2);

	vm_hw_connected[id] = 1;
	vm_hw_list[id] = hardware;
	
	return id;
}

void vm_hw_unregister(vm_t* vm, uint16_t id)
{
	vm_hw_connected[id] = 0;
}

void vm_hw_interrupt(vm_t* vm, uint16_t index)
{
	printf("Interrupting 0x%04X\n", index);
	vm_hw_list[index].handler(vm);
}

uint16_t vm_hw_count(vm_t* vm)
{
	uint16_t i = 0;
	for(i = 0; i < HW_MAX; i++) {
		if(vm_hw_connected[i] == 0) 
			return i;
	}
}

hw_t vm_hw_get_device(vm_t* vm, uint16_t index) {
	return vm_hw_list[index];
}

void lem1802_interrupt(vm_t* vm) {
	printf("Hello from the interrupt!\n");
}

void vm_hw_register_lem1802(vm_t* vm) 
{
	hw_t screen;
	
	screen.id_1 = 0xDEAD;
	screen.id_2 = 0xBEEF;
	screen.c = 0xFACE;
	screen.x = 0x1234;
	screen.y = 0x1337;
	screen.handler = &lem1802_interrupt;

	vm_hw_register(vm, screen);
}
