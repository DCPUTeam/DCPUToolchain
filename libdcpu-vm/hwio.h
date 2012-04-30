/**

	File:           hwio.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-vm

	Authors:        James Rhodes

	Description:    Declares macros and functions used to
	                host the virtual screen and keyboard.

**/

#ifndef __DCPUHWIO_H
#define __DCPUHWIO_H

#include <libtcod.h>
#include "dcpu.h"

#define KB_CLEAR_BUFFER 0
#define KB_STORE_IN_REG 1
#define KB_COMPARE		2
#define KB_INTERRUPTS	3

void vm_hw_io_init(vm_t* vm, uint16_t pos);
void vm_hw_io_free(vm_t* vm);

#endif
