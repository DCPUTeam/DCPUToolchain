/**

	File:		hwio.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
			emulator.

**/

#include <stdio.h>
#include <time.h>
#include <debug.h>
#include "hw.h"
#include "hwtimer.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

uint16_t message = 0x0;
uint16_t clock_enabled = 0;
uint16_t clock_ticks = 0;
float clock_interval = 0;
float clock_base = 0;
uint16_t hook_id = 0;
uint16_t hw_id = 0;

void vm_hw_timer_cycle(vm_t* vm, uint16_t pos, void* ud)
{
	float clock_current = (float)clock() / (float)CLOCKS_PER_SEC;
	if (clock_enabled == 1)
	{
		if (clock_current > clock_base + clock_interval)
		{
			clock_ticks++;
			clock_base += clock_interval;
			if (message > 0x0)
				vm_interrupt(vm, message);
		}
	}
}

void vm_hw_timer_interrupt(vm_t* vm, void* ud)
{
	switch (vm->registers[REG_A])
	{
		case TIMER_SET_ENABLED:
			if (vm->registers[REG_B] == 0x0)
				clock_enabled = 0;
			else
			{
				if (vm->registers[REG_B] > 60) break;
				clock_base = (float)clock() / (float)CLOCKS_PER_SEC;
				clock_interval = 1.f;
				clock_interval /= 60.f;
				clock_interval *= (float)(int)(vm->registers[REG_B]);
				clock_enabled = 1;
			}

			break;

		case TIMER_GET_ELAPSED:
			vm->registers[REG_C] = clock_ticks;
			clock_ticks = 0;
			break;

		case TIMER_SET_INTERRUPT:
			message = vm->registers[REG_B];
			break;
	}
}

void vm_hw_timer_init(vm_t* vm)
{
	hw_t timer;

	timer.id = 0x12D0B402;
	timer.version = 0x0001;
	timer.manufacturer = 0x00000000;
	timer.handler = &vm_hw_timer_interrupt;

	hook_id = vm_hook_register(vm, &vm_hw_timer_cycle, HOOK_ON_POST_CYCLE, NULL);
	hw_id = vm_hw_register(vm, timer);
}

void vm_hw_timer_free(vm_t* vm)
{
	vm_hook_unregister(vm, hook_id);
	vm_hw_unregister(vm, hw_id);
}
