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

#include "dcpu.h"

struct timer_hardware 
{
	hw_t device;
	vm_t* vm;

	uint16_t hook_id;
	uint16_t hw_id;

	int clock_target;
	int clock_ticks;

	uint16_t message;
};

#define TIMER_ID		0x12D0B402

#define TIMER_SET_ENABLED	0
#define TIMER_GET_ELAPSED	1
#define TIMER_SET_INTERRUPT	2

void vm_hw_timer_init(vm_t* vm);
void vm_hw_timer_cycle(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_timer_free(void* ud);

#endif
