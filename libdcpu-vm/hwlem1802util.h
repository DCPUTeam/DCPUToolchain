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

void vm_hw_lem1802_util_raw2rgb(uint16_t raw, unsigned char * rgb);
uint16_t vm_hw_lem1802_util_rgb2raw(unsigned char * rgb);
int vm_hw_lem1802_util_loadpng(int* out_width, int* out_height, int* out_has_alpha, unsigned char** outData);

#endif


