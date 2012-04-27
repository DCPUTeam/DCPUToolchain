/**

	File:			hwio.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
					emulator.

**/

#include <stdio.h>
#include "hw.h"
#include "hwtimer.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

uint32_t timer_tick = 0;
uint32_t base_frequency = 0;
uint16_t message = 0x0;
uint16_t clock_enabled = 0;

void vm_hw_timer_cycle(vm_t* vm, uint16_t pos)
{
	if(clock_enabled == 1)
	{
		timer_tick++;
		if (timer_tick > base_frequency) // processor runs at 100khz, timer interrupt triggers 60 times a second
		{
			if(message > 0x0)
			{
				vm_op_int(vm, message);
			}
			timer_tick = 0;
		}
	}
}

void vm_hw_timer_interrupt(vm_t* vm)
{
	uint16_t requested_action = vm_resolve_value(vm, REG_A, 0);
	uint16_t val_b = vm_resolve_value(vm, REG_B, 0);
	
	switch(requested_action) {
		case 0x0:
			if(val_b == 0x0)
			{
				clock_enabled = 0;
			}
			else
			{
				if(val_b > 60) break;
				base_frequency = 100000 / (60 / (int)val_b);
				clock_enabled = 1;

			}

			break;
		case 0x1:
			// TODO
			break;
		case 0x2:
			message = val_b;
			break;
	}
}

void vm_hw_timer_init(vm_t* vm)
{
	hw_t timer;
	
	timer.id_1 = 0x12d0;
	timer.id_2 = 0xb402;
	timer.c = 0xFACE;
	timer.x = 0x1234;
	timer.y = 0x1337;
	timer.handler = &vm_hw_timer_interrupt;

	vm_hook_register(vm, &vm_hw_timer_cycle, HOOK_ON_CYCLE);
	vm_hw_register(vm, timer);
}

void vm_hw_timer_free(vm_t* vm)
{
}
