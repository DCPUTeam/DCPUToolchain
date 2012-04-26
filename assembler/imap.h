/**

	File:			aout.h

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Declares functions and structures for the instruction
					and register maps.

**/

#ifndef __DCPU_ASM_IMAP_H
#define __DCPU_ASM_IMAP_H

#include <stdint.h>
#include "dcpu.h"

#define BRACKETS_UNSUPPORTED 0xFFFF
#define VALUE_NEXT_UNSUPPORTED 0xFFFE

struct instruction_mapping
{
	const char* name;
	uint16_t opcode;
	uint16_t nbopcode;
};

struct register_mapping
{
	const char* name;
	uint16_t value;
};

struct instruction_mapping* get_instruction_by_name(char* name);
struct register_mapping* get_register_by_name(char* name, bool bracketed);
struct register_mapping* get_register_by_name_next(char* name);
void strupper(char *str);

#endif