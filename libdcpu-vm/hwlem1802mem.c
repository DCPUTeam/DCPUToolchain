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
#include <GL/glfw3.h>
#include <stdlib.h>
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
/// The assigned font location, defaulting to the
/// internal font.
///
uint16_t font_location = 0;

///
/// The cloned LibTCOD image, used to easily adjust
/// the font in memory and then reset it back into
/// LibTCOD.
///
unsigned char * font_image = NULL;

///
/// Another cloned LibTCOD image, used to dump the
/// default into RAM when required.
///
uint16_t font_default[HW_LEM1802_FONT_MEMSIZE];

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
	int has_alpha, font_img_width, font_img_height;

	// Determine the font path.
	font_path = osutil_getarg0path();
#ifdef WIN32
	bconcat(font_path, bfromcstr("\\defaultfont.png"));
#else
	bconcat(font_path, bfromcstr("/defaultfont.png"));
#endif


	// load image with default font
	if (!vm_hw_lem1802_util_loadpng(font_path->data, &font_img_width, &font_img_height, &has_alpha, &font_image)) {
		printf("Error loading default font.");
		exit( EXIT_FAILURE );
	}
	
	// load default font from loaded image
	vm_hw_lem1802_mem_load_default_font();
	font_char_width = HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH;
	font_char_height = HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT;
	
	// Set the custom font based on the path.
	/*
	TCOD_console_set_custom_font(font_path->data, TCOD_FONT_LAYOUT_ASCII_INCOL, 0, 0);
	

	// Start LibTCOD.
#if TCOD_HEXVERSION > 0x010500
	TCOD_console_init_root(HW_LEM1802_SCREEN_WIDTH + 2, HW_LEM1802_SCREEN_HEIGHT + 2, "Toolchain Emulator", false, TCOD_RENDERER_SDL);
#else
	TCOD_console_init_root(HW_LEM1802_SCREEN_WIDTH + 2, HW_LEM1802_SCREEN_HEIGHT + 2, "Toolchain Emulator", false);
#endif
	TCOD_sys_set_fps(10000);
	font_image = TCOD_image_load(font_path->data);
	font_default = TCOD_image_load(font_path->data);
	TCOD_sys_get_char_size(&font_char_width, &font_char_height);

	*/

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
/// @param vm The virtual machine.
/// @param rgb The RGB color to use in rendering.
///
void vm_hw_lem1802_mem_get_palette_color(vm_t* vm, uint16_t idx, unsigned char * rgb)
{
	if (palette_location == HW_LEM1802_INTERNAL)
	{
		// Use the internal palette.
		vm_hw_lem1802_util_raw2rgb(palette_internal[idx], rgb);
	}
	else
	{
		// Use the custom palette.
		vm_hw_lem1802_util_raw2rgb(vm->ram[palette_location + idx], rgb);
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
/// @param vm The virtual machine.
///
void vm_hw_lem1802_mem_set_font(vm_t* vm, uint16_t pos)
{
	uint16_t i;

	// Set the memory-mapped font location.
	font_location = pos;

	// If the new font location is not 0, we must
	// resynchronise the font.
	if (font_location != HW_LEM1802_INTERNAL)
	{
		// Synchronise the font.
		for (i = 0; i < 0x100; i++)
			vm_hw_lem1802_write(vm, font_location + i, NULL);
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


void vm_hw_lem1802_mem_put_char_to_screen(vm_t* vm, uint16_t val, unsigned int screenX, unsigned int screenY, unsigned char * texture, int blink_on) {
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
	show_foreground = (blink_bit & blink_on) | (1-blink_bit);
	
	// get color palette color
	vm_hw_lem1802_mem_get_palette_color(vm, fore, foreclr);
	vm_hw_lem1802_mem_get_palette_color(vm, back, backclr);

	
	// TODO get correct font position
	fontloc = vm_hw_lem1802_mem_get_font();
	if (fontloc == 0)
	{
		fontval_word1 = font_default[2*chr];
		fontval_word2 = font_default[2*chr+1];
	} else {
		fontval_word1 = vm->ram[fontloc+2*chr];
		fontval_word2 = vm->ram[fontloc+2*chr+1];
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
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)] = foreclr[0];
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+1] = foreclr[1];
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+2] = foreclr[2];
			} else {
				// foreground
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)] = backclr[0];
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+1] = backclr[1];
				texture[HW_LEM1802_SCREEN_RGB_COOR(x,y)+2] = backclr[2];
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
	first = font_default[2*idx];
	second = font_default[2*idx+1];
	
	/*
	unsigned int i = 0, x = 0, y = 0, fx = 0, fy = 0;
	TCOD_color_t color_white = { 255, 255, 255 };
	TCOD_image_t char_image = NULL;
	uint16_t char_width, char_height;
	uint16_t ax = 0, ay = 0;
	uint16_t first = 0;
	uint16_t second = 0;
	uint16_t result = 0;
	TCOD_color_t color;

	// Get the font character width / height.
	char_width = vm_hw_lem1802_mem_get_font_char_width();
	char_height = vm_hw_lem1802_mem_get_font_char_height();
	char_image = vm_hw_lem1802_mem_get_default_font_image();

	// Work out the position of the character in the font.
	fx = idx / 16 * char_width;
	fy = idx % 16 * char_height;

	// For each pixel in the image, grab it's on / off value
	// from the memory location.
	for (x = 0; x < 4; x++)
	{
		// Loop through each row.
		for (y = 0; y < 8; y++)
		{
			// TEMPORARY: Some loaded fonts may have characters wider
			//	      or higher than the addressable font size and thus
			//	      we must skip over intermediate pixels.
			for (ax = 0; ax < char_width; ax += char_width / HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH)
			{
				for (ay = 0; ay < char_height; ay += char_height / HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT)
				{
					// Determine whether the pixel is on or off.
					color = TCOD_image_get_pixel(char_image, (fx + x) * char_width / HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH + ax, ay - (fy + y + 1) * char_height / HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT);
					if (color.r == color_white.r && color.g == color_white.g && color.b == color_white.b)
						result = 1;
					else
						result = 0;

					// Shift across 8-bits if needed.
					if (x == 0 || x == 2)
						result = result << 8;

					// Shift back based on the row.
					result = result >> y;

					// Set in the correct upper or lower field.
					if (x == 0 || x == 1)
						first += result;
					else
						second += result;
				}
			}
		}
	}
	* */

	// Combine first and second values into
	// a single uint32_t and return.
	return ((uint32_t)first << 16) + (uint32_t)second;
}

/*
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
/// Returns the default LibTCOD image.
///
/// Returns the defaultLibTCOD image.
///
/// @return The font image.
///
TCOD_image_t vm_hw_lem1802_mem_get_default_font_image()
{
	return font_default;
}
* 
* */

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

	// Set the new screen position.
	screen_location = pos;

	// TODO : clear screen in case of disconnect!

	/*

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
			vm_hw_lem1802_write(vm, screen_location + i, NULL);
	}
	* */
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

