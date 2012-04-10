/**

	File:			treloc.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Defines public functions for writing out
					relocation tables.

**/

#include <stdint.h>
#include <stdlib.h>
#include "treloc.h"
#include "aout.h"
#include "aerr.h"

uint16_t reloc_data[RELOC_MAXIMUM_ENTRIES];
uint16_t reloc_count = 0;

uint16_t treloc_init(struct aout_byte* start)
{
	struct aout_byte* current;
	uint32_t mem_index;

	// Write out the table (maximum 2000 entries).
	reloc_count = 0;
	current = start;
	mem_index = 0;
	while (current != NULL)
	{
		if (current->type != AOUT_TYPE_NORMAL)
		{
			current = current->next;
			continue;
		}
		if (current->label == NULL)
			mem_index += 1;
		if (current->label_replace != NULL)
		{
			fprintf(stderr, "RELOC [0x%04X] 0x%04X (points to %s)\n", reloc_count, mem_index, current->label_replace);
			reloc_data[reloc_count] = mem_index;
			reloc_count += 1;
			if (reloc_count == RELOC_MAXIMUM_ENTRIES)
				ahalt(ERR_RELOCATION_TABLE_TOO_LARGE, NULL);
		}
		current = current->next;
	}

	// Return how much code should be offset by.
	return RELOC_OFFSET;
}

void treloc_write(FILE* out)
{
	uint16_t i, inst;

	RELOC_WRITE_OP(OP_SET, PC, NXT_LIT);
	RELOC_WRITE_RAW(RELOC_OFFSET);
	RELOC_WRITE_RAW(RELOC_MAGIC);
	RELOC_WRITE_RAW(RELOC_VERSION);
	RELOC_WRITE_RAW(reloc_count);
	for (i = 0; i < reloc_count; i += 1)
	{
		RELOC_WRITE_RAW(reloc_data[i]);
	}
}