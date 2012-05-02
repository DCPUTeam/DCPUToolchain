/**

	File:		hwio.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
			emulator.

**/

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <string.h>
#include "hw.h"
#include "hwio.h"
#include "hwioascii.h"
#include "dcpuhook.h"
#include "dcpubase.h"
#include "dcpuops.h"

extern char* path;

uint16_t vm_cycle_update;

bool kb_legacy = false;
uint32_t input_index = 0;

uint16_t last_key = 0x0;
uint16_t interrupt_message = 0x0;

void vm_hw_io_cycle(vm_t* vm, uint16_t pos)
{
	TCOD_key_t key;
	uint16_t ascii;

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
			last_key = ascii;

			if (interrupt_message != 0)
				vm_op_int(vm, interrupt_message);

			if (kb_legacy != false)
				vm->ram[0x9000] = last_key;
		}
	}
}

void vm_hwio_interrupt(vm_t* vm)
{
	uint16_t requested_action = vm_resolve_value(vm, REG_A, 0);
	uint16_t val_b = vm_resolve_value(vm, REG_B, 0);
	uint16_t* store_c = vm_internal_get_store(vm, REG_C, POS__);

	// Switch off legacy mode if we ever
	// get sent an interrupt.
	kb_legacy = false;

	// Handle interrupt.
	switch (requested_action)
	{
		case KB_CLEAR_BUFFER:
			last_key = 0;
			break;
		case KB_STORE_IN_REG:
			*store_c = last_key;
			last_key = 0x0;
			break;
		case KB_COMPARE:
			*store_c = val_b == last_key ? 1 : 0;
			break;
		case KB_INTERRUPTS:
			if (val_b != 0)
				interrupt_message = val_b;
			else
				interrupt_message = 0;
	}
}

void vm_hw_io_init(vm_t* vm)
{
	hw_t keyboard;

	keyboard.id = 0x7349F615;
	keyboard.version = 0x1802;
	keyboard.manufacturer = 0x1C6C8B36;
	keyboard.handler = &vm_hwio_interrupt;

	// Register hooks.
	vm_cycle_update = vm_hook_register(vm, &vm_hw_io_cycle, HOOK_ON_CYCLE);
	vm_hw_register(vm, keyboard);
}

void vm_hw_io_set_legacy(bool legacy)
{
	// Set legacy keyboard mode; allows for
	// easy access to IO from within the C compiler
	// which doesn't automatically handle hardware.
	kb_legacy = legacy;
}

void vm_hw_io_free(vm_t* vm)
{
	// Unregister hooks.
	vm_hook_unregister(vm, vm_cycle_update);

	// Free TCOD memory.
	TCOD_console_delete(NULL);
}
