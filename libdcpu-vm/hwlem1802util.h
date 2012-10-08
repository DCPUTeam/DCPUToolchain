/**

	File:		lem1802util.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman
			Patrick Flick

	Description:	Declares utility functions for LEM1802.

**/

#ifndef __DCPU_HW_LEM1802_UTIL_H
#define __DCPU_HW_LEM1802_UTIL_H

#include <stdint.h>
#define png_infopp_NULL (png_infopp)NULL
#define png_voidp_NULL (png_voidp)NULL

void vm_hw_lem1802_util_raw2rgb(uint16_t raw, unsigned char * rgb);
uint16_t vm_hw_lem1802_util_rgb2raw(unsigned char * rgb);

#endif


