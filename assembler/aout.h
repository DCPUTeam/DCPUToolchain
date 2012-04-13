/**

	File:			aout.h

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Declares functions and structures used to emit
					bytecode and raw values to RAM images.

**/

#ifndef __DCPU_ASM_AOUT_H
#define __DCPU_ASM_AOUT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "dcpu.h"

#define AOUT_TYPE_NORMAL 0
#define AOUT_TYPE_METADATA_EXTENSION 1
#define AOUT_TYPE_METADATA_INCBIN 2
#define AOUT_TYPE_METADATA_ORIGIN 3

struct aout_byte
{
	uint16_t type;
	uint16_t opcode;
	uint16_t a;
	uint16_t b;
	struct aout_byte* next;
	struct aout_byte* prev;
	bool raw_used;
	uint16_t raw;
	char* label;
	char* label_replace;
};

extern struct aout_byte* start;
extern struct aout_byte* end;

struct aout_byte* aout_create_opcode(uint16_t opcode, uint16_t a, uint16_t b);
struct aout_byte* aout_create_raw(uint16_t raw);
struct aout_byte* aout_create_label(char* name);
struct aout_byte* aout_create_label_replace(char* name);
struct aout_byte* aout_create_metadata_extension(char* name);
struct aout_byte* aout_create_metadata_incbin(char* path);
struct aout_byte* aout_create_metadata_origin(uint16_t address);
void aout_emit(struct aout_byte* byte);
void aout_write(FILE* out, bool relocatable);

#endif