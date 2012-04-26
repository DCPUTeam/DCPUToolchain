/**

	File:			textn.h

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Declares public functions for writing out
					extension tables.

**/

#ifndef __DCPU_ASM_TEXTN_H
#define __DCPU_ASM_TEXTN_H

#include <stdint.h>
#include <stdio.h>
#include "aout.h"

#define EXTENSION_MAGIC 0x4321
#define EXTENSION_VERSION 0x0001
#define EXTENSION_OFFSET extension_offset + 5
#define EXTENSION_MAXIMUM_ENTRIES 50
#define EXTENSION_WRITE_OP(opcode, a, b); \
	inst = INSTRUCTION_CREATE(opcode, a, b); \
	fwrite(&inst, sizeof(uint16_t), 1, out);
#define EXTENSION_WRITE_RAW(raw); \
	inst = raw; \
	fwrite(&inst, sizeof(uint16_t), 1, out);

char* textn_verify_name(char* name);
uint16_t textn_init(struct aout_byte* start);
void textn_write(FILE* out);

#endif