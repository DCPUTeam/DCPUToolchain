/**

	File:			ldata.h

	Project:		DCPU-16 Tools
	Component:		Linker

	Authors:		James Rhodes

	Description:	Declares structures and functions used to
					retrieve the linker table from an object
					file.

**/

#ifndef __DCPU_LNK_LDATA_H
#define __DCPU_LNK_LDATA_H

#include <stdint.h>
#include <stdio.h>

#define LABEL_END 0
#define LABEL_PROVIDED 1
#define LABEL_REQUIRED 2

struct ldata_entry
{
	char* label;
	uint8_t mode;
	uint16_t address;
};

struct ldata_entry* ldata_read(FILE* in);

#endif