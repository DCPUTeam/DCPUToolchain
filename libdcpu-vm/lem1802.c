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
#include "lem1802.h"
#include "hwioascii.h"
#include "dcpuhook.h"
#include "dcpubase.h"

extern bstring path;

uint32_t screen_tick = 0;
uint32_t screen_width = 32;
uint32_t screen_height = 12;
TCOD_image_t char_image;
int char_width, char_height;
uint32_t char_addressable_width = 4;
uint32_t char_addressable_height = 8;
const TCOD_color_t color_white = { 255, 255, 255 };
const TCOD_color_t color_black = { 0, 0, 0 };
uint16_t vm_write_update, vm_cycle_update;

uint16_t base_screen = 0x8000;
uint16_t base_font = 0x8180;
uint16_t base_border = 0x8280;

void vm_lem1802_update(vm_t* vm, uint16_t pos)
{
	unsigned int i = 0, x = 0, y = 0, fx = 0, fy = 0, ax = 0, ay = 0;
	uint16_t val, fore, back, chr;
	TCOD_color_t foreclr, backclr;

	// Are we updating a general cell?
	if (pos >= base_screen && pos <= base_screen + 0x17f)
	{
		// Read memory value.
		i = pos;
		val = vm->ram[i];

		// Get foreground, background and character components.
		fore = (val & 0xF000) >> 12;
		back = (val & 0x0F00) >> 8;
		chr = (val & 0x00FF);

		// Create TCOD colours.
		foreclr.r = 255 * ((fore & 0x4) >> 2) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.g = 255 * ((fore & 0x2) >> 1) / ((1 - ((fore & 0x8) >> 3)) + 1);
		foreclr.b = 255 * (fore & 0x1)	      / ((1 - ((fore & 0x8) >> 3)) + 1);
		backclr.r = 255 * ((back & 0x4) >> 2) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.g = 255 * ((back & 0x2) >> 1) / ((1 - ((back & 0x8) >> 3)) + 1);
		backclr.b = 255 * (back & 0x1)	      / ((1 - ((back & 0x8) >> 3)) + 1);

		// Calculate X / Y.
		x = (i - base_screen) % screen_width;
		y = (i - base_screen) / screen_width;

		// Update TCOD.
		TCOD_console_put_char_ex(NULL, x + 1, y + 1, chr,
					 foreclr,
					 backclr
					);
	}
	// Are we updating a font character?
	else if (pos >= base_font && pos <= base_font + 0x100)
	{
		// Work out the position of the character in the font.
		i = (pos - base_font) / 2;
		fx = i / 16 * char_width;
		fy = i % 16 * char_height;

		// For each pixel in the image, grab it's on / off value
		// from the memory location.
		for (x = 0; x < 4; x++)
		{
			// Read value.
			if (x == 0 || x == 1)
				val = vm->ram[base_font + (i * 2)];
			else
				val = vm->ram[base_font + (i * 2) + 1];

			printf("char_update: %x\n", val);

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
}

void lem1802_set_border(uint16_t val)
{
	unsigned int i = 0, x = 0, y = 0, fx = 0, fy = 0, ax = 0, ay = 0;
	uint16_t fore, back, chr;
	TCOD_color_t foreclr, backclr;

	// Get foreground, background and character components.
	fore = (val & 0xF000) >> 12;
	back = (val & 0x0F00) >> 8;
	chr = (val & 0x00FF);

	// Create TCOD colours.
	foreclr.r = 255 * ((fore & 0x4) >> 2) / ((1 - ((fore & 0x8) >> 3)) + 1);
	foreclr.g = 255 * ((fore & 0x2) >> 1) / ((1 - ((fore & 0x8) >> 3)) + 1);
	foreclr.b = 255 * (fore & 0x1)	      / ((1 - ((fore & 0x8) >> 3)) + 1);
	backclr.r = 255 * ((back & 0x4) >> 2) / ((1 - ((back & 0x8) >> 3)) + 1);
	backclr.g = 255 * ((back & 0x2) >> 1) / ((1 - ((back & 0x8) >> 3)) + 1);
	backclr.b = 255 * (back & 0x1)	      / ((1 - ((back & 0x8) >> 3)) + 1);

	// Redraw frame.
#if TCOD_HEXVERSION > 0x010500

	for (x = 0; x < screen_width + 2; x++)	TCOD_console_set_char_background(NULL, x, 0, foreclr, TCOD_BKGND_SET);

	for (x = 0; x < screen_width + 2; x++)	TCOD_console_set_char_background(NULL, x, screen_height + 1, foreclr, TCOD_BKGND_SET);

	for (y = 1; y < screen_height + 1; y++) TCOD_console_set_char_background(NULL, 0, y, foreclr, TCOD_BKGND_SET);

	for (y = 1; y < screen_height + 1; y++) TCOD_console_set_char_background(NULL, screen_width + 1, y, foreclr, TCOD_BKGND_SET);

#else
	/*	for (x = 0; x < screen_width + 2; x++)	TCOD_console_set_back(NULL, x, 0, foreclr, TCOD_BKGND_SET);
		for (x = 0; x < screen_width + 2; x++)	TCOD_console_set_back(NULL, x, screen_height + 1, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_back(NULL, 0, y, foreclr, TCOD_BKGND_SET);
		for (y = 1; y < screen_height + 1; y++) TCOD_console_set_back(NULL, screen_width + 1, y, foreclr, TCOD_BKGND_SET);
		*/
#endif
}

void vm_lem1802_cycle(vm_t* vm, uint16_t pos)
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
		// Stop VM.
		vm->halted = true;
		return;
	}

	// Update screen.
	TCOD_console_flush();
}

void vm_lem1802_load_font(vm_t* vm)
{
	int i = 0;

	for (i = 0; i < base_font + 0x100; i++) vm_lem1802_update(vm, base_font + i);
}

void vm_lem1802_interrupt(vm_t* vm)
{
	uint16_t requested_action = vm_resolve_value(vm, REG_A, 0);
	uint16_t val_b = vm_resolve_value(vm, REG_B, 0);

	switch (requested_action)
	{
		case MEM_MAP_SCREEN:
			if (val_b == 0)
			{
				vm->halted = true; // TODO: actually turn the screen off instead of halting the whole thing
			}
			else
			{
				base_screen = val_b;
			}

			break;

		case MEM_MAP_FONT:
			if (val_b == 0)
			{
				// load original font
			}
			else
			{
				base_font = val_b;
				vm_lem1802_load_font(vm);
			}

			break;

		case MEM_MAP_PALETTE:
			// TODO
			break;

		case SET_BORDER_COLOR:
			lem1802_set_border(val_b); // TODO: palette
			break;
	}
}

void vm_lem1802_init(vm_t* vm, uint16_t pos)
{
	bstring imagePath;
	hw_t screen;

	screen.id_1 = 0xf615;
	screen.id_2 = 0x7349;
	screen.c = 0xFACE;
	screen.x = 0x1234;
	screen.y = 0x1337;
	screen.handler = &vm_lem1802_interrupt;

	imagePath = bstrcpy(path);
	bconcat(imagePath, bfromcstr("/terminal.png"));

	TCOD_console_set_custom_font(bstr2cstr(imagePath, (char) '\0'), TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);
	// Load TCOD.

#if TCOD_HEXVERSION > 0x010500
	TCOD_console_init_root(screen_width + 2, screen_height + 2, "Toolchain Emulator", false, TCOD_RENDERER_SDL);
#else
	TCOD_console_init_root(screen_width + 2, screen_height + 2, "Toolchain Emulator", false);
#endif
	TCOD_console_set_keyboard_repeat(200, 10);
	TCOD_sys_set_fps(10000);
	char_image = TCOD_image_load(bstr2cstr(path, (char) '\0'));
	TCOD_sys_get_char_size(&char_width, &char_height);

	// Register hooks.
	vm_write_update = vm_hook_register(vm, &vm_lem1802_update, HOOK_ON_WRITE);
	vm_cycle_update = vm_hook_register(vm, &vm_lem1802_cycle, HOOK_ON_CYCLE);

	vm_hw_register(vm, screen);
}

