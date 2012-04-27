/**

	File:			ldata.c

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Defines functions used to retrieve the
					linker table from an object file.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

void ldata_write(FILE* out, struct ldata_entry* entry)
{
	// The label part of the structure might not be
	// 256 bytes in memory, so we need to copy it first.
	char* cpy = malloc(256);
	memset(cpy, 0, 256);
	if (entry->label != NULL)
	{
		if (strlen(entry->label) + 1 < 256)
			memcpy(cpy, entry->label, strlen(entry->label) + 1);
		else
			memcpy(cpy, entry->label, 256);
	}
	cpy[255] = '\0'; // Safety.

	// Read the character information (256 bytes).
	fwrite(cpy, 1, 256, out);
	fwrite(&entry->mode, sizeof(uint8_t), 1, out);
	fwrite(&entry->address, sizeof(uint16_t), 1, out);
}
