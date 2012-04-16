/**

	File:			ldata.c

	Project:		DCPU-16 Tools
	Component:		Linker

	Authors:		James Rhodes

	Description:	Defines functions used to retrieve the
					linker table from an object file.

**/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "ldata.h"

struct ldata_entry* ldata_read(FILE* in)
{
	struct ldata_entry* result = malloc(sizeof(struct ldata_entry));

	// Read the character information (256 bytes).
	fread(result->label, 1, 256, in);
	fread(&result->mode, sizeof(uint8_t), 1, in);
	fread(&result->address, sizeof(uint16_t), 1, in);
	result->label[255] = '\0'; // Safety.

	return result;
}