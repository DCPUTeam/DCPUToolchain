//
// DCPU Assembler by James Rhodes
//
// Instruction map.
//

#ifndef __DCPU_ASM_IMAP_H
#define __DCPU_ASM_IMAP_H

#include <stdint.h>
#include "dcpu.h"

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

#endif