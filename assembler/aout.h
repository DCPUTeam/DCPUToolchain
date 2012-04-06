//
// DCPU Assembler by James Rhodes
//
// Instruction linked list.
//

#ifndef __DCPU_ASM_AOUT_H
#define __DCPU_ASM_AOUT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "dcpu.h"

struct aout_byte
{
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
void aout_emit(struct aout_byte* byte);
void aout_write(FILE* out);

#endif