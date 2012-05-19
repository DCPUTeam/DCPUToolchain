/**

	File:		lem1802util.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman

	Description:	Declares utility functions for LEM1802.

**/

#ifndef __DCPU_HW_LEM1802_UTIL_H
#define __DCPU_HW_LEM1802_UTIL_H

#include <stdint.h>
#include <libtcod.h>

TCOD_color_t vm_hw_lem1802_util_get_color(uint16_t raw);

#endif


