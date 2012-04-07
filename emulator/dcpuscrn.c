/**

	File:			dcpuscrn.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen for the
					emulator.

**/

#include "dcpuscrn.h"

void vm_scrn_init(vm_t* vm)
{
	TCOD_console_init_root(VM_SCREEN_WIDTH, VM_SCREEN_HEIGHT, "Hach-Que's DCPU-16 Emulator", false);
}

void vm_scrn_cycle(vm_t* vm)
{
	unsigned int i = 0, x = 0, y = 0;
	uint16_t val, fore, back, chr;
	TCOD_color_t foreclr, backclr;

	// Check to see if window is closed.
	if (TCOD_console_is_window_closed())
	{
		// Stop VM.
		vm->halted = true;
		return;
	}
	else
		// We don't care about keypresses, but it keeps the window updated properly.
		TCOD_console_check_for_keypress(0);

	// Loop over memory.
	for (x = 0; x < VM_SCREEN_WIDTH; x += 1)
		for (y = 0; y < VM_SCREEN_HEIGHT; y += 1)
		{
			// Read memory value.
			i = 0x8000 + x + y * VM_SCREEN_WIDTH;
			val = vm->ram[i];
			
			// Get foreground, background and character components.
			fore = (val & 0xF000) >> 12;
			back = (val & 0x0F00) >> 8;
			chr =  (val & 0x00FF);

			// Create TCOD colours.
			foreclr.r = 255 * ((fore & 0x4) >> 2);
			foreclr.g = 255 * ((fore & 0x2) >> 1);
			foreclr.b = 255 * (fore & 0x1);
			backclr.r = 255 * ((back & 0x4) >> 2);
			backclr.g = 255 * ((back & 0x2) >> 1);
			backclr.b = 255 * (back & 0x1);

			// Update TCOD.
			TCOD_console_put_char_ex(NULL, x, y, chr,
				foreclr,
				backclr
				);
		}

	// Update screen.
	TCOD_console_flush();
}

void vm_scrn_free(vm_t* vm)
{
	TCOD_console_delete(NULL);
}