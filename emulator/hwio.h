/**

	File:			hwio.h

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Declares macros and functions used to
					host the virtual screen and keyboard.

**/

#ifndef __DCPUHWIO_H
#define __DCPUHWIO_H

#include <libtcod.h>
#include "dcpu.h"

void vm_hw_io_init(vm_t* vm);
void vm_hw_io_cycle(vm_t* vm);
void vm_hw_io_free(vm_t* vm);

#endif