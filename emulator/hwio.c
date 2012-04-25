/**

	File:			hwio.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
					emulator.

**/

#include <stdio.h>
#include <string.h>
#include "hwio.h"
#include "hwioascii.h"
#include "dcpuhook.h"

extern char* path;

uint16_t vm_write_update, vm_cycle_update;

uint32_t kb_tick = 0;
uint32_t input_index = 0;

void vm_hw_io_cycle(vm_t* vm, uint16_t pos)
{
	TCOD_key_t key;
	uint16_t ascii;

	// Only continue if we have done 2500 ticks.
	if (kb_tick < 2500)
	{
		kb_tick += 1;
		return;
	}
	else
		kb_tick = 0;

	// Check to see if window is closed.
	if (TCOD_console_is_window_closed())
	{
		// Stop VM.
		vm->halted = true;
		return;
	}
	else
	{
		// We don't care about keypresses, but it keeps the window updated properly.
		key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);

		// If there was a keypress, put it into the buffer if we can.
		ascii = vm_hw_io_ascii_get_map(key);
		if (ascii != 0)
		{
			if (vm->ram[(uint16_t)(0x9000 + input_index)] == 0x0)
			{
				vm->ram[0x9010] = 0x9000 + input_index;
				vm->ram[(uint16_t)(0x9000 + input_index++)] = ascii;
				if (input_index > 0xf) input_index = 0x0;
			}
		}
	}

	// Update screen.
	TCOD_console_flush();
}

void vm_hw_io_init(vm_t* vm, uint16_t pos)
{
	// Initialize input buffer.
	vm->ram[0x9010] = 0x9000 + input_index;
	// Register hooks.
	vm_cycle_update = vm_hook_register(vm, &vm_hw_io_cycle, HOOK_ON_CYCLE);
}

void vm_hw_io_free(vm_t* vm)
{
	// Unregister hooks.
	vm_hook_unregister(vm, vm_write_update);
	vm_hook_unregister(vm, vm_cycle_update);

	// Free TCOD memory.
	TCOD_console_delete(NULL);
}
