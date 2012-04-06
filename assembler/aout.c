//
// DCPU Assembler by James Rhodes
//
// Instruction linked list.
//

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "aout.h";

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

void aout_write(FILE* out)
{
	struct aout_byte* current_outer;
	struct aout_byte* current_inner;
	uint32_t mem_index;
	uint16_t inst;

	// First go through and replace all labels that need to be.
	current_outer = start;
	while (current_outer != NULL)
	{
		if (current_outer->label_replace != NULL)
		{
			current_inner = start;
			mem_index = 0;
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
		}
		current_outer = current_outer->next;
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