/**

	File:		lem1802util.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman

	Description:	Defines utility functions for LEM1802.

**/

#include "hwlem1802util.h"

///
/// Converts a raw 16-bit color into the LibTCOD representation.
///
/// @param raw The raw 16-bit color.
/// @return The LibTCOD color used for rendering.
///
TCOD_color_t vm_hw_lem1802_util_get_color(uint16_t raw)
{
	TCOD_color_t result;

	// Mathematical!
	result.r = ((raw & 0xF00) >> 8) * 16;
	result.g = ((raw & 0x0F0) >> 4) * 16;
	result.b = ((raw & 0x00F) >> 0) * 16;

	return result;
}

