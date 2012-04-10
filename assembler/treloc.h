/**

	File:			treloc.h

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Declares public functions for writing out
					relocation tables.

**/

#ifndef __DCPU_ASM_TRELOC_H
#define __DCPU_ASM_TRELOC_H

#include <stdio.h>

#define RELOC_MAGIC 0x1234
#define RELOC_VERSION 0x0001
#define RELOC_OFFSET reloc_count + 5
#define RELOC_MAXIMUM_ENTRIES 2000
#define RELOC_WRITE_OP(opcode, a, b); \
	inst = INSTRUCTION_CREATE(opcode, a, b); \
	fwrite(&inst, sizeof(uint16_t), 1, out);
#define RELOC_WRITE_RAW(raw); \
	inst = raw; \
	fwrite(&inst, sizeof(uint16_t), 1, out);

uint16_t treloc_init(struct aout_byte* start);
void treloc_write(FILE* out);

#endif