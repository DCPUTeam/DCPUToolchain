/**

	File:		lem1802.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman

	Description:	Handles LEM1802 functions

**/

#include <stdio.h>
#include <string.h>
#include <bstring.h>

#include "hw.h"
#include "hwlem1802.h"
#include "hwlem1802mem.h"
#include "hwioascii.h"
#include "dcpuhook.h"
#include "dcpubase.h"

uint32_t screen_tick = 0;
uint16_t hook_write_id;
uint16_t hook_cycle_id;
uint16_t hook_break_id;
uint16_t hw_id;

void vm_hw_lem1802_write(vm_t* vm, uint16_t pos, void* ud)
{
	unsigned int i = 0, x = 0, y = 0, fx = 0, fy = 0;
	uint16_t val, fore, back, chr;
	TCOD_color_t foreclr, backclr;
	uint16_t location_font, location_screen;
	uint16_t char_width, char_height;
	TCOD_image_t char_image = NULL;
	uint16_t ax = 0, ay = 0;
	TCOD_color_t color_black = { 0, 0, 0 };
	TCOD_color_t color_white = { 255, 255, 255 };

	// Get the current memory-mapping positions.
	location_font = vm_hw_lem1802_mem_get_font(vm);
	location_screen = vm_hw_lem1802_mem_get_screen(vm);

	// Get the font character width / height.
	char_width = vm_hw_lem1802_mem_get_font_char_width();
	char_height = vm_hw_lem1802_mem_get_font_char_height();
	char_image = vm_hw_lem1802_mem_get_font_image();

	// Are we updating a general cell?
	if (location_screen != 0 && pos >= location_screen && pos <= location_screen + 0x17F)
	{
		// Read memory value.
		i = pos;
		val = vm->ram[i];

		// Get foreground, background and character components.
		fore = (val & 0xF000) >> 12;
		back = (val & 0x0F00) >> 8;
		chr = (val & 0x00FF);

		// Create TCOD colours.
		foreclr = vm_hw_lem1802_mem_get_palette_color(vm, fore);
		backclr = vm_hw_lem1802_mem_get_palette_color(vm, back);

		// Calculate X / Y.
		x = (i - location_screen) % HW_LEM1802_SCREEN_WIDTH;
		y = (i - location_screen) / HW_LEM1802_SCREEN_WIDTH;

		// Update TCOD.
		TCOD_console_put_char_ex(NULL, x + 1, y + 1, chr,
					 foreclr,
					 backclr
					);
	}
	// Are we updating a font character?
	else if (location_font != 0 && pos >= location_font && pos <= location_font + 0x100)
	{
		// First adjust the position so that it aligns on the start
		// of a character.
		if ((pos - location_font) % 2 != 0)
			pos -= 1;

		// Work out the position of the character in the font.
		i = (pos - location_font) / 2;
		fx = i / 16 * char_width;
		fy = i % 16 * char_height;

		// For each pixel in the image, grab it's on / off value
		// from the memory location.
		for (x = 0; x < 4; x++)
		{
			// Read value.
			if (x == 0 || x == 1)
				val = vm->ram[pos];
			else
				val = vm->ram[pos + 1];

			// Get upper or lower component of value.
			if (x == 0 || x == 2)
				val = val >> 8;
			else
				val = (val << 8) >> 8;

			// Loop through each column.
			for (y = 0; y < 8; y++)
			{
				// TEMPORARY: If the character width is 8 pixels wide, we are using
				//	      the standard libTCOD font and hence when we write
				//	      a single DCPU pixel, we actually need to write two
				//	      adjacent libTCOD pixels.
				for (ax = 0; ax < char_width / HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH; ax += 1)
				{
					for (ay = 0; ay < char_height / HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT; ay += 1)
					{

						// If bit is true, write to the pixel white.
						if (((val & (0x1 << (7 - y))) >> (7 - y)) == 0x1)
							TCOD_image_put_pixel(char_image, (fx + x) * char_width / HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH + ax, ay - (fy + y + 1) * char_height / HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT, color_white);
						else
							TCOD_image_put_pixel(char_image, (fx + x) * char_width / HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH + ax, ay - (fy + y + 1) * char_height / HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT, color_black);
					}
				}
			}
		}

		// Update the bitmap.
		TCOD_sys_update_char(i, fx / char_width, fy / char_height, char_image, fx, fy);
	}
}

void vm_hw_lem1802_set_border(vm_t* vm, uint16_t idx)
{
	unsigned int x = 0, y = 0;
	TCOD_color_t clr;

	// Create the TCOD color.
	clr = vm_hw_lem1802_mem_get_palette_color(vm, idx & 0xF);

	// Redraw frame.
#if TCOD_HEXVERSION > 0x010500
	for (x = 0; x < HW_LEM1802_SCREEN_WIDTH + 2; x++)
		TCOD_console_set_char_background(NULL, x, 0, clr, TCOD_BKGND_SET);
	for (x = 0; x < HW_LEM1802_SCREEN_WIDTH + 2; x++)
		TCOD_console_set_char_background(NULL, x, HW_LEM1802_SCREEN_HEIGHT + 1, clr, TCOD_BKGND_SET);
	for (y = 1; y < HW_LEM1802_SCREEN_HEIGHT + 1; y++)
		TCOD_console_set_char_background(NULL, 0, y, clr, TCOD_BKGND_SET);
	for (y = 1; y < HW_LEM1802_SCREEN_HEIGHT + 1; y++)
		TCOD_console_set_char_background(NULL, HW_LEM1802_SCREEN_WIDTH + 1, y, clr, TCOD_BKGND_SET);
#else
	for (x = 0; x < HW_LEM1802_SCREEN_WIDTH + 2; x++)
		TCOD_console_set_back(NULL, x, 0, clr, TCOD_BKGND_SET);
	for (x = 0; x < HW_LEM1802_SCREEN_WIDTH + 2; x++)
		TCOD_console_set_back(NULL, x, HW_LEM1802_SCREEN_HEIGHT + 1, clr, TCOD_BKGND_SET);
	for (y = 1; y < HW_LEM1802_SCREEN_HEIGHT + 1; y++)
		TCOD_console_set_back(NULL, 0, y, clr, TCOD_BKGND_SET);
	for (y = 1; y < HW_LEM1802_SCREEN_HEIGHT + 1; y++)
		TCOD_console_set_back(NULL, HW_LEM1802_SCREEN_WIDTH + 1, y, clr, TCOD_BKGND_SET);
#endif
}

void vm_hw_lem1802_cycle(vm_t* vm, uint16_t pos, void* ud)
{
	// Only continue if we have done 2500 ticks.
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
		// FIXME: Cleanly detach the hardware from the
		// virtual machine in the case of the debugger,
		// rather than stopping the VM.

		// Stop VM.
		vm->halted = true;
		return;
	}

	// Update screen.
	TCOD_console_flush();

	// Required for proper redraw event to occur.  Since it's
	// non-blocking, it shouldn't consume a character from the
	// input, but this is not confirmed!
	TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
}

void vm_hw_lem1802_break(vm_t* vm, uint16_t pos, void* ud)
{
	screen_tick = 2500;
	vm_hw_lem1802_cycle(vm, pos, ud);
}

void vm_hw_lem1802_interrupt(vm_t* vm, void* ud)
{
	uint16_t requested_action = vm_resolve_value(vm, REG_A, 0);
	uint16_t val_b = vm_resolve_value(vm, REG_B, 0);

	switch (requested_action)
	{
		case LEM1802_MEM_MAP_SCREEN:
			vm_hw_lem1802_mem_set_screen(vm, val_b);
			break;

		case LEM1802_MEM_MAP_FONT:
			vm_hw_lem1802_mem_set_font(vm, val_b);
			break;

		case LEM1802_MEM_MAP_PALETTE:
			vm_hw_lem1802_mem_set_palette(vm, val_b);
			break;

		case LEM1802_SET_BORDER_COLOR:
			vm_hw_lem1802_set_border(vm, val_b);
			break;

		case LEM1802_MEM_DUMP_FONT:
			// TODO
			break;

		case LEM1802_MEM_DUMP_PALETTE:
			// TODO
			break;
	}
}

void vm_hw_lem1802_init(vm_t* vm)
{
	hw_t screen;

	// Initialize the memory for LEM1802.
	vm_hw_lem1802_mem_init(vm);
	// Set up the LEM1802 hardware information.
	screen.id = 0x7349F615;
	screen.version = 0x1802;
	screen.manufacturer = 0x1C6C8B36;
	screen.handler = &vm_hw_lem1802_interrupt;

	// Register hooks.
	hook_write_id = vm_hook_register(vm, &vm_hw_lem1802_write, HOOK_ON_WRITE, NULL);
	hook_cycle_id = vm_hook_register(vm, &vm_hw_lem1802_cycle, HOOK_ON_CYCLE, NULL);
	hook_break_id = vm_hook_register(vm, &vm_hw_lem1802_break, HOOK_ON_BREAK, NULL);
	hw_id = vm_hw_register(vm, screen);
}

void vm_hw_lem1802_free(vm_t* vm)
{
	// Unregister hooks.
	vm_hook_unregister(vm, hook_write_id);
	vm_hook_unregister(vm, hook_cycle_id);
	vm_hook_unregister(vm, hook_break_id);
	vm_hw_unregister(vm, hw_id);
}