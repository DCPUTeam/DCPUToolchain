/**

	File:			dcpubase.h

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Declares functions that handle core
					functionality of the virtual machine.

**/

#ifndef __DCPUBASE_H
#define __DCPUBASE_H

#include "dcpu.h"

void vm_halt(vm_t* vm, const char* message, ...);
uint16_t vm_consume_word(vm_t* vm);
uint16_t vm_resolve_value(vm_t* vm, uint16_t val);
void vm_cycle(vm_t* vm);

#endif