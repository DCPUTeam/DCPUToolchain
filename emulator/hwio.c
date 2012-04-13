/**

	File:			hwio.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
					emulator.

**/

#include "hwio.h"
#include "dcpuhook.h"

uint32_t screen_tick = 0;
uint32_t screen_width = 32;
uint32_t screen_height = 12;

void vm_hw_io_update(vm_t* vm, uint16_t pos);

void vm_hw_io_init(vm_t* vm)
{
	TCOD_console_init_root(screen_width + 2, screen_height + 2, "DCPU-16 Tools Emulator", false);
	TCOD_sys_set_fps(10000);
	vm_hook_register(vm, &vm_hw_io_update, 0);
}

void vm_hw_io_update(vm_t* vm, uint16_t pos)
{
	unsigned int i = 0, x = 0, y = 0;
	uint16_t val, fore, back, chr;
	TCOD_color_t foreclr, backclr;
	TCOD_image_t image;

	if (!(pos >= 0x8000 && pos <= 0x8000 + (screen_width * screen_height)))
		return;
	
	// Are we updating a general cell?
	if (pos >= 0x8000 && pos <= 0x817f)
	{
		// Read memory value.
		i = pos;
		val = vm->ram[i];
			
		// Get foreground, background and character components.
		fore = (val & 0xF000) >> 12;
		back = (val & 0x0F00) >> 8;
		chr =  (val & 0x00FF);

		// Create TCOD colours.
		foreclr.r = 255 - (255 * ((fore & 0x4) >> 2));
		foreclr.g = 255 - (255 * ((fore & 0x2) >> 1));
		foreclr.b = 255 - (255 * (fore & 0x1));
		backclr.r = 255 * ((back & 0x4) >> 2);
		backclr.g = 255 * ((back & 0x2) >> 1);
		backclr.b = 255 * (back & 0x1);

		// Calculate X / Y.
		x = (i - 0x8000) % screen_width;
		y = (i - 0x8000) / screen_width;

		// Update TCOD.
		TCOD_console_put_char_ex(NULL, x + 1, y + 1, chr,
			foreclr,
			backclr
			);
	}
	// Are we updating a font character?
	else if (pos > 0x817f && pos < 0x8240)
	{
		// First create the TCOD image to hold our new font.
		image = TCOD_image_new(4, 8);

		// For each pixel in the image, grab it's on / off value
		// from the memory location.
		for (x = 0; x < 4; x++)
		{
			for (y = 0; y < 8; y++)
			{
				// Read value.
				if (x == 0 || x == 1)
					val = (vm->ram[(uint16_t)(pos - 0x817f / 2)] & (0x1 << ((x * 8) + y))) >> (0x1 << ((x * 8) + y));
				else
					val = (vm->ram[(uint16_t)(pos - 0x817f / 2 + 1)] & (0x1 << (((x - 2) * 8) + y))) >> (0x1 << (((x - 2) * 8) + y));

				// If val == 1, write to the pixel white.
				if (val == 1)
					TCOD_image_put_pixel(image, x, y, TCOD_white);
			}
		}

		TCOD_sys_update_char(pos - 0x817f / 2, 0, 0, image, 0, 0);
	}
	// Are we updating the screen border?
	else if (pos == 0x8240)
	{
		// Read memory value.
		i = pos;
		val = vm->ram[i];

		// Get foreground, background and character components.
		fore = (val & 0xF000) >> 12;
		back = (val & 0x0F00) >> 8;
		chr =  (val & 0x00FF);

		// Create TCOD colours.
		foreclr.r = 255 - (255 * ((fore & 0x4) >> 2));
		foreclr.g = 255 - (255 * ((fore & 0x2) >> 1));
		foreclr.b = 255 - (255 * (fore & 0x1));
		backclr.r = 255 * ((back & 0x4) >> 2);
		backclr.g = 255 * ((back & 0x2) >> 1);
		backclr.b = 255 * (back & 0x1);

		// Redraw frame
		TCOD_console_print_frame(NULL, x, y, screen_width + 2, screen_height + 2, false, TCOD_BKGND_SET, NULL);
	}
}

void vm_hw_io_cycle(vm_t* vm)
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

void vm_hw_io_free(vm_t* vm)
{
	TCOD_console_delete(NULL);
}