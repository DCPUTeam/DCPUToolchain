/**

	File:		lem1802.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes
			José manuel Díez

	Description:	Handles LEM1802 functions

**/

#ifndef __LEM1802_H
#define __LEM1802_H

#include <libtcod.h>
#include "dcpu.h"

#define MEM_MAP_SCREEN	 0
#define MEM_MAP_FONT	 1
#define MEM_MAP_PALETTE	 2
#define SET_BORDER_COLOR 3

void vm_lem1802_init(vm_t* vm, uint16_t pos);
void vm_lem1802_free(vm_t* vm);
void vm_lem1802_interrupt(vm_t* vm);

#endif
