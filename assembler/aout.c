//
// DCPU Assembler by James Rhodes
//
// Instruction linked list.
//

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "aout.h";
#include "aerr.h";
#include "dcpu.h";

struct aout_byte* start = NULL;
struct aout_byte* end = NULL;

struct aout_byte* aout_create_opcode(uint16_t opcode, uint16_t a, uint16_t b)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->opcode = opcode;
	byte->a = a;
	byte->b = b;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
	byte->label = NULL;
	byte->label_replace = NULL;
	if (opcode == 0 && a == 0)
		ahalt(ERR_OUTPUT_NULL, NULL);
	return byte;
}

struct aout_byte* aout_create_raw(uint16_t raw)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = true;
	byte->raw = raw;
	byte->label = NULL;
	byte->label_replace = NULL;
	return byte;
}

struct aout_byte* aout_create_label(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
	byte->label = name;
	byte->label_replace = NULL;
	return byte;
}

struct aout_byte* aout_create_label_replace(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->label_replace = NULL;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = true;
	byte->raw = 0x0;
	byte->label = NULL;
	byte->label_replace = name;
	return byte;
}

void aout_emit(struct aout_byte* byte)
{
	if (start == NULL && end == NULL)
	{
		start = byte;
		end = byte;
		byte->next = NULL;
		byte->prev = NULL;
	}
	else
	{
		byte->next = NULL;
		byte->prev = end;
		end->next = byte;
		end = byte;
	}
}

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

void aout_write(FILE* out, bool relocatable)
{
	struct aout_byte* current_outer;
	struct aout_byte* current_inner;
	uint32_t mem_index, i;
	uint16_t inst;
	uint16_t reloc_data[RELOC_MAXIMUM_ENTRIES];
	uint16_t reloc_count = 0;

	// If relocatable, set up our relocation table (maximum 2000 entries).
	if (relocatable)
	{
		current_outer = start;
		mem_index = 0;
		while (current_outer != NULL)
		{
			if (current_outer->label == NULL)
				mem_index += 1;
			if (current_outer->label_replace != NULL)
			{
				printf("RELOC [0x%04X] 0x%04X (points to %s)\n", reloc_count, mem_index, current_outer->label_replace);
				reloc_data[reloc_count] = mem_index;
				reloc_count += 1;
				if (reloc_count == RELOC_MAXIMUM_ENTRIES)
					ahalt(ERR_RELOCATION_TABLE_TOO_LARGE, NULL);
			}
			current_outer = current_outer->next;
		}
	}

	// First go through and replace all labels that need to be.
	current_outer = start;
	while (current_outer != NULL)
	{
		if (current_outer->label_replace != NULL)
		{
			current_inner = start;
			mem_index = RELOC_OFFSET;
			while (current_inner != NULL)
			{
				if (current_inner->label == NULL)
					mem_index += 1;
				else if (strcmp(current_inner->label, current_outer->label_replace) == 0)
				{
					//current_outer->raw = 0xff88;
					current_outer->raw = mem_index;
					current_outer->label_replace = NULL;
					break;
				}
				current_inner = current_inner->next;
			}
			if (current_outer->label_replace != NULL)
				ahalt(ERR_LABEL_NOT_FOUND, current_outer->label_replace);
		}
		current_outer = current_outer->next;
	}

	// If relocatable, write our start system.
	if (relocatable)
	{
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
	
	// Now write to the file.
	current_outer = start;
	while (current_outer != NULL)
	{
		if (current_outer->raw_used == true)
		{
			inst = current_outer->raw;
			fwrite(&inst, sizeof(uint16_t), 1, out);
		}
		else if (current_outer->label == NULL)
		{
			inst = INSTRUCTION_CREATE(current_outer->opcode, current_outer->a, current_outer->b);
			fwrite(&inst, sizeof(uint16_t), 1, out);
		}

		current_outer = current_outer->next;
	}

	fflush(out);
}