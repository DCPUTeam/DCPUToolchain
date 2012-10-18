/**

	File:		aout.h

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Declares functions and structures used to emit
			bytecode and raw values to RAM images.

**/

#ifndef __DCPU_ASM_AOUT_H
#define __DCPU_ASM_AOUT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simclist.h>
#include <ppexpr.h>
#include "dcpu.h"

#define AOUT_TYPE_NORMAL 0
#define AOUT_TYPE_METADATA_EXTENSION 1
#define AOUT_TYPE_METADATA_INCBIN 2
#define AOUT_TYPE_METADATA_ORIGIN 3
#define AOUT_TYPE_METADATA_EXPORT 4
#define AOUT_TYPE_METADATA_IMPORT 5
#define AOUT_TYPE_METADATA_SECTION 6
#define AOUT_TYPE_METADATA_OUTPUT 7
#define AOUT_TYPE_METADATA_SEEK 8

struct aout_byte
{
	uint16_t type;
	uint16_t opcode;
	uint16_t a;
	uint16_t b;
	struct aout_byte* next;
	struct aout_byte* prev;
	struct expr* expr;
	char* label;
	bool raw_used;
	uint16_t raw;
	list_t symbols;
};

extern struct aout_byte* start;
extern struct aout_byte* end;

struct aout_byte* aout_create_raw(uint16_t raw);
struct aout_byte* aout_create_opcode(uint16_t opcode, uint16_t a, uint16_t b);
struct aout_byte* aout_create_label(char* name);
struct aout_byte* aout_create_expr(struct expr* expression);
struct aout_byte* aout_create_metadata_extension(char* name);
struct aout_byte* aout_create_metadata_incbin(char* path);
struct aout_byte* aout_create_metadata_origin(uint16_t address);
struct aout_byte* aout_create_metadata_seek(uint16_t address);
struct aout_byte* aout_create_metadata_export(char* name);
struct aout_byte* aout_create_metadata_import(char* name);
struct aout_byte* aout_create_metadata_section(char* name);
struct aout_byte* aout_create_metadata_output(char* name);
struct aout_byte* aout_emit(struct aout_byte* byte);
uint16_t aout_write(FILE* out, bool relocatable, bool intermediate);

#endif
