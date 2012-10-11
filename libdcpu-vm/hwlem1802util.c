/**

	File:		lem1802util.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman
			Patrick Flick

	Description:	Defines utility functions for LEM1802.

**/

#include "hwlem1802util.h"
#include <stdlib.h>
#include <debug.h>
#include <png.h>

///
/// The raw bitmap data of the embedded font.
///
extern const char vm_hw_lem1802_font[];

///
/// The length of the raw bitmap data of the embedded font.
///
extern const size_t vm_hw_lem1802_font_len;

///
/// Converts a raw 16-bit color into RGB representation.
///
/// @param raw The raw 16-bit color.
/// @param rgb The RGB color used for rendering.
///
void vm_hw_lem1802_util_raw2rgb(uint16_t raw, unsigned char * rgb)
{
	// do repeated mapping:
	//     e.g. the lem color value 1111 -> 1111 1111
	//	    and 1010 -> 1010 1010
	//	    and 0000 -> 0000 0000
	// this way perfect white and perfect black is representable
	//
	// doing this mathematical by multiplying with 0b10001 (17)
	rgb[0] = ((raw & 0xF00) >> 8) * 17;
	rgb[1] = ((raw & 0x0F0) >> 4) * 17;
	rgb[2] = ((raw & 0x00F) >> 0) * 17;
}

///
/// Converts a RGB color into a raw 16-bit color.
///
/// @param color The RGB color.
/// @return The raw value.
///
uint16_t vm_hw_lem1802_util_rgb2raw(unsigned char * rgb)
{
	uint16_t lem_color = 0x0;
	lem_color |= (rgb[0] >> 4) << 8;
	lem_color |= (rgb[1] >> 4) << 4;
	lem_color |= (rgb[2] >> 4);
	return lem_color;
}

void read_png_from_memory(png_structp pngPtr, png_bytep data, png_size_t length)
{
    static int idx = 0;
    memcpy(data, vm_hw_lem1802_font + idx, length);
    idx += length;
}

int vm_hw_lem1802_util_loadpng(int* out_width, int* out_height, int* out_has_alpha, unsigned char** outData)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytepp row_pointers;
	unsigned int sig_read = 0;
	unsigned int row_bytes;
	int i;

	/* Create and initialize the png_struct
	* with the desired error handler
	* functions.  If you want to use the
	* default stderr and longjump method,
	* you can supply NULL for the last
	* three parameters.  We also supply the
	* the compiler header file version, so
	* that we know if the application
	* was compiled with a compatible version
	* of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL)
		return 0;

	/* Allocate/initialize the memory
	* for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 0;
	}

	/* Set error handling if you are
	* using the setjmp/longjmp method
	* (this is the normal method of
	* doing things with libpng).
	* REQUIRED unless you  set up
	* your own error handlers in
	* the png_create_read_struct()
	* earlier.
	*/
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated
		* with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		/* If we get here, we had a
		* problem reading the file */
		return 0;
	}

	// Read in from memory.
	png_set_read_fn(png_ptr, NULL, read_png_from_memory);

	/* If we have already
	* read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);

	/*
	* If you have enough memory to read
	* in the entire image at once, and
	* you need to specify only
	* transforms that can be controlled
	* with one of the PNG_TRANSFORM_*
	* bits (this presently excludes
	* dithering, filling, setting
	* background, and doing gamma
	* adjustment), then you can read the
	* entire image (including pixels)
	* into the info structure with this
	* call
	*
	* PNG_TRANSFORM_STRIP_16 |
	* PNG_TRANSFORM_PACKING  forces 8 bit
	* PNG_TRANSFORM_EXPAND forces to
	*  expand a palette into RGB
	*/
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	
	*out_width = png_get_image_width(png_ptr, info_ptr);
	*out_height = png_get_image_height(png_ptr, info_ptr);
	switch (png_get_color_type(png_ptr, info_ptr)) {
		case PNG_COLOR_TYPE_RGBA:
		*out_has_alpha = 1;
		break;
		case PNG_COLOR_TYPE_RGB:
		*out_has_alpha  = 0;
		break;
		default:
		printd(LEVEL_ERROR, "lem1802: error loading default font: color type not supported\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 0;
	}
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	*outData = (unsigned char*) malloc(row_bytes * (*out_height));

	row_pointers = png_get_rows(png_ptr, info_ptr);

	for (i = 0; i < (*out_height); i++) {
		memcpy(*outData+(row_bytes * i), row_pointers[i], row_bytes);
	}

	/* Clean up after the read,
	* and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* That's it */
	return 1;
}



