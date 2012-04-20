/**

	File:			hwio.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Hosts the virtual screen and keyboard for the
					emulator.

**/

#include <stdio.h>
#include "hwio.h"
#include "hwioascii.h"
#include "dcpuhook.h"

uint32_t screen_tick = 0;
uint32_t screen_width = 32;
uint32_t screen_height = 12;
uint32_t input_index = 0;
TCOD_image_t char_image;
int char_width, char_height;
uint32_t char_addressable_width = 4;
uint32_t char_addressable_height = 8;
const TCOD_color_t color_white = { 255, 255, 255 };
const TCOD_color_t color_black = { 0, 0, 0 };

void vm_hw_io_update(vm_t* vm, uint16_t pos);

void vm_hw_io_init(vm_t* vm)
{
#if TCOD_HEXVERSION > 0x010500
	TCOD_console_init_root(screen_width + 2, screen_height + 2, "DCPU-16 Tools Emulator", false, TCOD_RENDERER_SDL);
#else
	TCOD_console_init_root(screen_width + 2, screen_height + 2, "DCPU-16 Tools Emulator", false);
#endif
	TCOD_console_set_keyboard_repeat(200, 10);
	TCOD_sys_set_fps(10000);
	vm_hook_register(vm, &vm_hw_io_update, 0);
	char_image = TCOD_image_load("terminal.png");
	TCOD_sys_get_char_size(&char_width, &char_height);
	vm->ram[0x9010] = 0x9000 + input_index;
}

void vm_hw_io_update(vm_t* vm, uint16_t pos)
{
	unsigned int i = 0, x = 0, y = 0, fx = 0, fy = 0, ax = 0, ay = 0;
	uint16_t val, fore, back, brgt, chr;
	TCOD_color_t foreclr, backclr;

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
		foreclr.r = 255 * ((fore & 0x4) >> 2) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.g = 255 * ((fore & 0x2) >> 1) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.b = 255 * (fore & 0x1)        / ((1 - ((fore & 0x8) >> 3)) + 1);
		backclr.r = 255 * ((back & 0x4) >> 2) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.g = 255 * ((back & 0x2) >> 1) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.b = 255 * (back & 0x1)        / ((1 - ((back & 0x8) >> 3)) + 1);

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
	else if (pos > 0x817f && pos < 0x8280)
	{
		// Work out the position of the character in the font.
		i = (pos - 0x8180) / 2;
		fx = i / 16 * char_width;
		fy = i % 16 * char_height;
		
		// For each pixel in the image, grab it's on / off value
		// from the memory location.
		for (x = 0; x < 4; x++)
		{
			// Read value.
			if (x == 0 || x == 1)
				val = vm->ram[0x8180 + (i * 2)];
			else
				val = vm->ram[0x8180 + (i * 2) + 1];

			// Get upper or lower component of value.
			if (x == 0 || x == 2)
				val = val >> 8;
			else
				val = (val << 8) >> 8;

			// Loop through each column.
			for (y = 0; y < 8; y++)
			{
				// TEMPORARY: If the character width is 8 pixels wide, we are using
				//            the standard libTCOD font and hence when we write
				//            a single DCPU pixel, we actually need to write two
				//            adjacent libTCOD pixels.
				for (ax = 0; ax < char_width / char_addressable_width; ax += 1)
				{
					for (ay = 0; ay < char_height / char_addressable_height; ay += 1)
					{
						// If bit is true, write to the pixel white.
						if (((val & (0x1 << (8 - y))) >> (8 - y)) == 0x1)
							TCOD_image_put_pixel(char_image, (fx + x) * char_width / char_addressable_width + ax, (fy + y) * char_height / char_addressable_height + ay, color_white);
						else
							TCOD_image_put_pixel(char_image, (fx + x) * char_width / char_addressable_width + ax, (fy + y) * char_height / char_addressable_height + ay, color_black);
					}
				}
			}
		}

		// Update the bitmap.
		TCOD_sys_update_char(i, fx / char_width, fy / char_height, char_image, fx, fy);
	}
	// Are we updating the screen border?
	else if (pos == 0x8280)
	{
		// Read memory value.
		i = pos;
		val = vm->ram[i];

		// Get foreground, background and character components.
		fore = (val & 0xF000) >> 12;
		back = (val & 0x0F00) >> 8;
		chr =  (val & 0x00FF);

		// Create TCOD colours.
		foreclr.r = 255 * ((fore & 0x4) >> 2) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.g = 255 * ((fore & 0x2) >> 1) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.b = 255 * (fore & 0x1)        / ((1 - ((fore & 0x8) >> 3)) + 1);
		backclr.r = 255 * ((back & 0x4) >> 2) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.g = 255 * ((back & 0x2) >> 1) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.b = 255 * (back & 0x1)        / ((1 - ((back & 0x8) >> 3)) + 1);

		// Redraw frame.
#if TCOD_HEXVERSION > 0x010500
		for (x = 0; x < screen_width + 2; x++)  TCOD_console_set_char_background(NULL, x, 0, foreclr, TCOD_BKGND_SET);
		for (x = 0; x < screen_width + 2; x++)  TCOD_console_set_char_background(NULL, x, screen_height + 1, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_char_background(NULL, 0, y, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_char_background(NULL, screen_width + 1, y, foreclr, TCOD_BKGND_SET);
#else
		for (x = 0; x < screen_width + 2; x++)  TCOD_console_set_back(NULL, x, 0, foreclr, TCOD_BKGND_SET);
		for (x = 0; x < screen_width + 2; x++)  TCOD_console_set_back(NULL, x, screen_height + 1, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_back(NULL, 0, y, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_back(NULL, screen_width + 1, y, foreclr, TCOD_BKGND_SET);
#endif
	}
}

void vm_hw_io_cycle(vm_t* vm)
{
	TCOD_key_t key;
	uint16_t ascii;

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

void vm_hw_io_free(vm_t* vm)
{
	TCOD_image_delete(char_image);
	TCOD_console_delete(NULL);
}
