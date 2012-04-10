/**

	File:			dcpuscrn.h

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Declares macros and functions used to
					host the virtual screen.

**/

#ifndef __DCPUSCRN_H
#define __DCPUSCRN_H

#include <libtcod.h>
#include "dcpu.h"

void vm_scrn_init(vm_t* vm, uint32_t width, uint32_t height);
void vm_scrn_cycle(vm_t* vm);
void vm_scrn_free(vm_t* vm);

#endif