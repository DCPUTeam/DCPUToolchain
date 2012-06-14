/**

	File:		treloc.h

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Declares public functions for writing out
			relocation tables.

**/

#ifndef __DCPU_ASM_TRELOC_H
#define __DCPU_ASM_TRELOC_H

#include <stdio.h>
#include "aout.h"

#define RELOC_MAGIC 0x1234
#define RELOC_VERSION 0x0001
#define RELOC_OFFSET reloc_count + 5
#define RELOC_MAXIMUM_ENTRIES 2000
#define RELOC_WRITE_OP(opcode, a, b); \
	inst = INSTRUCTION_CREATE(opcode, a, b); \
	iwrite(&inst, out);
#define RELOC_WRITE_RAW(raw); \
	inst = raw; \
	iwrite(&inst, out);

uint16_t treloc_init(struct aout_byte* start);
void treloc_write(FILE* out);

#endif
