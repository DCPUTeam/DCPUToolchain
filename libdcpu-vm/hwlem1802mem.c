/**

	File:		lem1802mem.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman

	Description:	Handles internal LEM1802 memory.

**/

#include <osutil.h>
#include "hwlem1802.h"
#include "hwlem1802mem.h"
#include "hwlem1802util.h"

///
/// The assigned palette location, defaulting to the
/// internal palette.
///
uint16_t palette_location = 0;

///
/// The internal palette used by LEM1802 when no
/// palette memory is assigned.
///
const uint16_t palette_internal[16] =
{
	/* 0b0000 */ 0x0000,
	/* 0b0001 */ 0x0007,
	/* 0b0010 */ 0x0070,
	/* 0b0011 */ 0x0077,
	/* 0b0100 */ 0x0700,
	/* 0b0101 */ 0x0707,
	/* 0b0110 */ 0x0770,
	/* 0b0111 */ 0x0777,
	/* 0b1000 */ 0x0000,
	/* 0b1001 */ 0x000F,
	/* 0b1010 */ 0x00F0,
	/* 0b1011 */ 0x00FF,
	/* 0b1100 */ 0x0F00,
	/* 0b1101 */ 0x0F0F,
	/* 0b1110 */ 0x0FF0,
	/* 0b1111 */ 0x0FFF
};

///
/// The assigned font location, defaulting to the
/// internal font.
///
uint16_t font_location = 0;

///
/// The cloned LibTCOD image, used to easily adjust
/// the font in memory and then reset it back into
/// LibTCOD.
///
TCOD_image_t font_image = NULL;

///
/// The width of a single character in the font image.
///
int font_char_width = 0;

///
/// The height of a single character in the font image.
///
int font_char_height = 0;

///
/// The assigned screen location.
///
uint16_t screen_location = 0;

///
/// Initializes the internal LEM1802 memory.
///
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_init(vm_t* vm)
{
	bstring font_path;

	// Determine the font path.
	font_path = osutil_getarg0path();
	bconcat(font_path, bfromcstr("/terminal.png"));

	// Set the custom font based on the path.
	TCOD_console_set_custom_font(font_path->data, TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);

	// Start LibTCOD.
#if TCOD_HEXVERSION > 0x010500
	TCOD_console_init_root(HW_LEM1802_SCREEN_WIDTH + 2, HW_LEM1802_SCREEN_HEIGHT + 2, "Toolchain Emulator", false, TCOD_RENDERER_SDL);
#else
	TCOD_console_init_root(HW_LEM1802_SCREEN_WIDTH + 2, HW_LEM1802_SCREEN_HEIGHT + 2, "Toolchain Emulator", false);
#endif
	TCOD_console_set_keyboard_repeat(200, 10);
	TCOD_sys_set_fps(10000);
	font_image = TCOD_image_load(font_path->data);
	TCOD_sys_get_char_size(&font_char_width, &font_char_height);

	// Free memory.
	bdestroy(font_path);
}

///
/// Frees the internal LEM1802 memory.
///
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_free(vm_t* vm)
{
}

///
/// Sets the palette location used for resolving palette colors.
///
/// Sets the palette location used for resolving palette colors.
/// If the passed value is 0, then the internal palette is used
/// for the screen.
///
/// @param pos The position that the palette is mapped at.
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_set_palette(vm_t* vm, uint16_t pos)
{
	palette_location = pos;
}

///
/// Returns the LibTCOD color based on the required index,
/// automatically determining whether or not the value should
/// be retrieved from the internal palette or the VM's RAM.
///
/// @param vm The virtual machine.
/// @return The LibTCOD color to use in rendering.
///
TCOD_color_t vm_hw_lem1802_mem_get_palette_color(vm_t* vm, uint16_t idx)
{
	if (palette_location == HW_LEM1802_INTERNAL)
	{
		// Use the internal palette.
		return vm_hw_lem1802_util_get_color(palette_internal[idx]);
	}
	else
	{
		// Use the custom palette.
		return vm_hw_lem1802_util_get_color(vm->ram[palette_location + idx]);
	}
}

///
/// Sets the font location used for resolving font characters.
///
/// Sets the font location used for resolving font characters.
/// If the passed value is 0, then the internal font is used
/// for the screen.
///
/// @param pos The position that the font is mapped at.
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_set_font(vm_t* vm, uint16_t pos)
{
	uint16_t i;

	// Set the memory-mapped font location.
	font_location = pos;

	// If the new font location is not 0, we must
	// resynchronise the font.
	if (font_location == HW_LEM1802_INTERNAL)
	{
		// Synchronise the font.
		for (i = 0; i < font_location + 0x100; i++)
			vm_hw_lem1802_write(vm, font_location + i);
	}
}

///
/// Returns the position that the font is memory-mapped at.
///
/// Returns the position that the font is memory-mapped at,
/// or 0 if there is no memory-mapped font.
///
/// @return The position that the font is mapped at.
///
uint16_t vm_hw_lem1802_mem_get_font()
{
	return font_location;
}

///
/// Returns the width of the characters in the font image.
///
/// @return The width of the characters in the font image.
///
uint16_t vm_hw_lem1802_mem_get_font_char_width()
{
	return font_char_width;
}

///
/// Returns the height of the characters in the font image.
///
/// @return The height of the characters in the font image.
///
uint16_t vm_hw_lem1802_mem_get_font_char_height()
{
	return font_char_height;
}

///
/// Returns the LibTCOD image.
///
/// Returns the LibTCOD image.	This should probably be factored
/// out to setting pixel functions in the font directly and hiding
/// the exact implementation from other files.
///
/// @return The font image.
///
TCOD_image_t vm_hw_lem1802_mem_get_font_image()
{
	return font_image;
}

///
/// Sets the screen location used for memory mapping values.
///
/// Sets the screen location used for memory mapping values.
/// If the passed value is 0, then the screen is in a
/// disconnected state.
///
/// @param pos The position that the screen is mapped at.
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_set_screen(vm_t* vm, uint16_t pos)
{
	uint16_t x, y, i;
	TCOD_color_t empty = { 0, 0, 0 };

	// Set the new screen position.
	screen_location = pos;

	// If the new screen location is 0, we must clear
	// the screen.
	if (screen_location == HW_LEM1802_DISCONNECTED)
	{
		// Clear the screen.
		for (x = 0; x < HW_LEM1802_SCREEN_WIDTH; x++)
			for (y = 0; y < HW_LEM1802_SCREEN_HEIGHT; y++)
				TCOD_console_put_char_ex(NULL, x + 1, y + 1, ' ', empty, empty);
	}
	else
	{
		// Resynchronise with the VM's RAM.
		for (i = 0; i < screen_location + HW_LEM1802_SCREEN_MEMSIZE; i++)
			vm_hw_lem1802_write(vm, screen_location + i);
	}
}

///
/// Returns the position that the screen is memory-mapped at.
///
/// Returns the position that the screen is memory-mapped at,
/// or 0 if the screen is disconnected.
///
/// @return The position that the screen is mapped at.
///
uint16_t vm_hw_lem1802_mem_get_screen()
{
	return screen_location;
}