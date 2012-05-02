/**

	File:		hwtimer.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes

	Description:	Declares macros and functions used to
			host the interrupt timer.

**/

#ifndef __DCPUHWTIMER_H
#define __DCPUHWTIMER_H

#include <libtcod.h>
#include "dcpu.h"

void vm_hw_timer_init(vm_t* vm);
void vm_hw_timer_cycle(vm_t* vm, uint16_t pos);
void vm_hw_timer_free(vm_t* vm);

#endif
