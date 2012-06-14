/**

	File:		iio.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Provides functions for reading and writing
			assembler instructions in the correct endianness.

**/

#ifndef __DCPU_IIO_H
#define __DCPU_IIO_H

#define IMODE_UNKNOWN 0
#define IMODE_LITTLE 1
#define IMODE_BIG 2

#include <stdio.h>
#include <stdint.h>

void isetmode(int mode);
size_t iwrite(const uint16_t* src, FILE* out);
size_t iread(uint16_t* dest, FILE* in);

#endif