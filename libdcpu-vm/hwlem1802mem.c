/**

	File:		lem1802mem.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman
			Patrick Flick

	Description:	Handles internal LEM1802 memory.

**/

#include <osutil.h>
#include <GL/glfw3.h>
#include <stdlib.h>
#include "hwlem1802.h"
#include "hwlem1802mem.h"
#include "hwlem1802util.h"

///
/// The internal palette used by LEM1802 when no
/// palette memory is assigned.
///
const uint16_t palette_internal[16] =
{
	/* 0b0000 */ 0x0000,
	/* 0b0001 */ 0x000A,
	/* 0b0010 */ 0x00A0,
	/* 0b0011 */ 0x00AA,
	/* 0b0100 */ 0x0A00,
	/* 0b0101 */ 0x0A0A,
	/* 0b0110 */ 0x0A50,
	/* 0b0111 */ 0x0AAA,
	/* 0b1000 */ 0x0555,
	/* 0b1001 */ 0x055F,
	/* 0b1010 */ 0x05F5,
	/* 0b1011 */ 0x05FF,
	/* 0b1100 */ 0x0F55,
	/* 0b1101 */ 0x0F5F,
	/* 0b1110 */ 0x0FF5,
	/* 0b1111 */ 0x0FFF
};

///
/// The default font is loaded as PNG image into this RGB image:
///
unsigned char * font_image = NULL;

///
/// Default font in LEM font representation.
///
uint16_t font_default[HW_LEM1802_FONT_MEMSIZE];


///
/// Initializes the internal LEM1802 memory.
///
/// @param hw The LEM1802 HW structure.
///
void vm_hw_lem1802_mem_init(struct lem1802_hardware* hw)
{
	bstring font_path;

	int has_alpha, font_img_width, font_img_height;

	// init ram locations
	hw->palette_location = HW_LEM1802_DEFAULT_PALETTE_LOC;
	hw->font_location = HW_LEM1802_DEFAULT_FONT_LOC;
	hw->screen_location = HW_LEM1802_DISCONNECTED;

	
	// only load default font, if it hasn't been loaded before
	// (first time monitor is opened)
	if (font_image == NULL)
	{
	
		// Determine the font path.
		font_path = osutil_getarg0path();
#ifdef WIN32
		bconcat(font_path, bfromcstr("\\defaultfont.png"));
#else
		bconcat(font_path, bfromcstr("/defaultfont.png"));
#endif

		// load image with default font
		if (!vm_hw_lem1802_util_loadpng(font_path->data, &font_img_width, &font_img_height, &has_alpha, &font_image))
		{
			printf("Error loading default font.");
			bdestroy(font_path);
			return;
		}
		
		// load default font from loaded image
		vm_hw_lem1802_mem_load_default_font();

		// Free memory.
		bdestroy(font_path);
	}
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
/// @param hw The LEM1802 HW structure.
///
void vm_hw_lem1802_mem_set_palette(struct lem1802_hardware* hw, uint16_t pos)
{
	hw->palette_location = pos;
}

void vm_hw_lem1802_mem_dump_default_palette(vm_t* vm, uint16_t pos)
{
	uint16_t idx;
	for (idx = 0; idx < 16; idx++)
	{
		vm->ram[pos + idx] = palette_internal[idx];
	}
}

///
/// Returns the RGB color based on the required index,
/// automatically determining whether or not the value should
/// be retrieved from the internal palette or the VM's RAM.
///
/// @param hw The LEM1802 HW structure.
/// @param rgb The RGB color to use in rendering.
///
void vm_hw_lem1802_mem_get_palette_color(struct lem1802_hardware* hw, uint16_t idx, unsigned char * rgb)
{
	if (hw->palette_location == HW_LEM1802_DEFAULT_PALETTE_LOC)
	{
		// Use the internal palette.
		vm_hw_lem1802_util_raw2rgb(palette_internal[idx], rgb);
	}
	else
	{
		// Use the custom palette.
		vm_hw_lem1802_util_raw2rgb(hw->vm->ram[hw->palette_location + idx], rgb);
	}
}

///
/// Returns the default color based on the required index.
///
/// @param idx The color index.
/// @param rgb The RGB color to use in rendering.
///
void vm_hw_lem1802_mem_get_default_palette_color(uint16_t idx, unsigned char * rgb)
{
	vm_hw_lem1802_util_raw2rgb(palette_internal[idx], rgb);
}

///
/// Sets the font location used for resolving font characters.
///
/// Sets the font location used for resolving font characters.
/// If the passed value is 0, then the internal font is used
/// for the screen.
///
/// @param pos The position that the font is mapped at.
/// @param hw The LEM1802 HW structure.
///
void vm_hw_lem1802_mem_set_font(struct lem1802_hardware* hw, uint16_t pos)
{
	uint16_t i;

	// Set the memory-mapped font location.
	hw->font_location = pos;

	// If the new font location is not 0, we must
	// resynchronise the font.
	// TODO maybe i dont need this anymore :)
	if (hw->font_location != HW_LEM1802_DEFAULT_FONT_LOC)
	{
		// Synchronise the font.
		for (i = 0; i < 0x100; i++)
			vm_hw_lem1802_write(hw->vm, hw->font_location + i, hw);
	}
}

///
/// Returns the position that the font is memory-mapped at.
///
/// Returns the position that the font is memory-mapped at,
/// or 0 if there is no memory-mapped font.
///
/// @param hw The LEM1802 HW structure.
/// @return The position that the font is mapped at.
///
uint16_t vm_hw_lem1802_mem_get_font(struct lem1802_hardware* hw)
{
	return hw->font_location;
}


void vm_hw_lem1802_mem_load_default_font() {
	unsigned int character, fontRow, fontCol, outputIndex;
	int imgTopLeftX, imgTopLeftY, x, y;
	uint16_t word1, word2, shift;
	
	
	for (character = 0; character < HW_LEM1802_FONT_NUM_CHARS; character++) {
		
		fontRow = character / HW_LEM1802_FONT_IMG_COLS;
		fontCol = character % HW_LEM1802_FONT_IMG_COLS;
		outputIndex = 2*character;
		
		imgTopLeftX = fontCol * HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH;
		imgTopLeftY = fontRow * HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT;
		
		// TODO do this in one pass, by starting with MSB first
		
		// get first word
		word1 = 0x0;
		shift = 0;
		for (x = imgTopLeftX+1; x >= imgTopLeftX; x--) {
			for (y = imgTopLeftY; y < imgTopLeftY+HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT; y++) {
				if (font_image[HW_LEM1802_FONT_RGB_COOR(x,y)] > 128) {
					// foreground
					word1 |= (0x1 << shift);
				}
				
				shift++;
			}
		}
		
		// get second word
		word2 = 0x0;
		shift = 0;
		for (x = imgTopLeftX+3; x >= imgTopLeftX+2; x--) {
			for (y = imgTopLeftY; y < imgTopLeftY+HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT; y++) {
				if (font_image[HW_LEM1802_FONT_RGB_COOR(x,y)] > 128) {
					// foreground
					word2 |= (0x1 << shift);
				}
				shift++;
			}
		}
		
		// save to default font
		font_default[2*character] = word1;
		font_default[2*character+1] = word2;
	}
}


void vm_hw_lem1802_mem_put_char_to_screen(struct lem1802_hardware* hw, uint16_t val, unsigned int screenX, unsigned int screenY) {
	uint16_t fore, back, chr, fontval_word1, fontval_word2;
	uint32_t fontval32bit;
	uint16_t fontloc;
	int blink_bit;
	int show_foreground;
	unsigned char foreclr[3];
	unsigned char backclr[3];
	unsigned int shift;
	int textureTopLeftX, textureTopLeftY, x, y;
	
	fore = (val & 0xF000) >> 12;
	back = (val & 0x0F00) >> 8;
	chr = (val & 0x007F);
	blink_bit = (val & 0x0080) >> 7;
	show_foreground = (blink_bit & hw->blink_status) | (1-blink_bit);
	
	// get color palette color
	vm_hw_lem1802_mem_get_palette_color(hw, fore, foreclr);
	vm_hw_lem1802_mem_get_palette_color(hw, back, backclr);
	
	// TODO get correct font position
	fontloc = vm_hw_lem1802_mem_get_font(hw);
	if (fontloc == HW_LEM1802_DEFAULT_FONT_LOC)
	{
		fontval_word1 = font_default[2*chr];
		fontval_word2 = font_default[2*chr+1];
	} else {
		fontval_word1 = hw->vm->ram[fontloc+2*chr];
		fontval_word2 = hw->vm->ram[fontloc+2*chr+1];
	}
	fontval32bit = (fontval_word1 << 16) | fontval_word2;
	
	// get coordinates of character in texture
	textureTopLeftX = screenX * HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH;
	textureTopLeftY = screenY * HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT;
	
	shift = 31;
	for (x = textureTopLeftX; x < textureTopLeftX+HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH; x++) {
		for (y = textureTopLeftY+HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT-1; y >= textureTopLeftY; y--) {
			if ((fontval32bit >> shift) & 1 & show_foreground) {
				// foreground
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)] = foreclr[0];
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+1] = foreclr[1];
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+2] = foreclr[2];
			} else {
				// foreground
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)] = backclr[0];
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+1] = backclr[1];
				hw->glfw_texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+2] = backclr[2];
			}
			shift--;
		}
	}
}

///
/// Returns a representation of a default character as it would
/// appear in the DCPU-16 RAM.
///
/// @param idx The index in the font to retrieve.
/// @return The 32-bit integer (which is stored as two 16-bit words) representation.
///
uint32_t vm_hw_lem1802_mem_get_font_default_representation(uint16_t idx)
{
	uint16_t first, second;
	
	// get font words
	first = font_default[2*idx];
	second = font_default[2*idx+1];

	// Combine first and second values into
	// a single uint32_t and return.
	return ((uint32_t)first << 16) + (uint32_t)second;
}


///
/// Sets the screen location used for memory mapping values.
///
/// Sets the screen location used for memory mapping values.
/// If the passed value is 0, then the screen is in a
/// disconnected state.
///
/// @param pos The position that the screen is mapped at.
/// @param hw The LEM1802 HW structure.
///
void vm_hw_lem1802_mem_set_screen(struct lem1802_hardware* hw, uint16_t pos)
{
	// Set the new screen position.
	hw->screen_location = pos;
	
	if (pos == 0)
		hw->screen_location = HW_LEM1802_DISCONNECTED;
}

///
/// Returns the position that the screen is memory-mapped at.
///
/// Returns the position that the screen is memory-mapped at,
/// or 0 if the screen is disconnected.
///
/// @param hw The LEM1802 HW structure.
/// @return The position that the screen is mapped at.
///
uint16_t vm_hw_lem1802_mem_get_screen(struct lem1802_hardware* hw)
{
	return hw->screen_location;
}

