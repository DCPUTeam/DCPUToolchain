/**

	File:			dcpuscrn.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen for the
					emulator.

**/

#include "dcpuscrn.h"
#include "dcpuhook.h"

uint32_t screen_tick = 0;
uint32_t screen_width = 0;
uint32_t screen_height = 0;

void vm_scrn_update(vm_t* vm, uint16_t pos);

void vm_scrn_init(vm_t* vm, uint32_t width, uint32_t height)
{
	TCOD_console_init_root(width, height, "DCPU-16 Tools Emulator", false);
	TCOD_sys_set_fps(10000);
	vm_hook_register(vm, &vm_scrn_update, 0);
	screen_width = width;
	screen_height = height;
}

void vm_scrn_update(vm_t* vm, uint16_t pos)
{
	unsigned int i = 0, x = 0, y = 0;
	uint16_t val, fore, back, chr;
	TCOD_color_t foreclr, backclr;

	if (!(pos >= 0x8000 && pos <= 0x8000 + (screen_width * screen_height)))
		return;
	
	// Read memory value.
	i = pos;
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

	// Calculate X / Y.
	x = (i - 0x8000) % screen_width;
	y = (i - 0x8000) / screen_width;

	// Update TCOD.
	TCOD_console_put_char_ex(NULL, x, y, chr,
		foreclr,
		backclr
		);
}

void vm_scrn_cycle(vm_t* vm)
{
	if (screen_tick < 2500)
	{
		screen_tick += 1;
		return;
	}
	else
		screen_tick = 0;

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

	// Update screen.
	TCOD_console_flush();
}

void vm_scrn_free(vm_t* vm)
{
	TCOD_console_delete(NULL);
}