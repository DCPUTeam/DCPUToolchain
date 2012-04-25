/**

	File:			hwio.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
					emulator.

**/

#include <stdio.h>
#include "hwtimer.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"

uint32_t timer_tick = 0;

void vm_hw_timer_cycle(vm_t* vm, uint16_t pos)
{
	timer_tick++;
	/*if (timer_tick > 100000 / 60) // processor runs at 100khz, timer interrupt triggers 60 times a second
	{
		vm_interrupt(vm, INT_TIMER);
		timer_tick = 0;
	}*/
}

void vm_hw_timer_init(vm_t* vm)
{
	vm_hook_register(vm, &vm_hw_timer_cycle, HOOK_ON_CYCLE);
}

void vm_hw_timer_free(vm_t* vm)
{
}
