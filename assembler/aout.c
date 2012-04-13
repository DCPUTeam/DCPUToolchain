/**

	File:			aout.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Implements outputting emitted opcodes and raw
					values into RAM images.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "aout.h"
#include "aerr.h"
#include "dcpu.h"
#include "treloc.h"
#include "textn.h"
#include "pp.h"

struct aout_byte* start = NULL;
struct aout_byte* end = NULL;
extern char* fileloc;

struct aout_byte* aout_create_opcode(uint16_t opcode, uint16_t a, uint16_t b)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_NORMAL;
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
	byte->type = AOUT_TYPE_NORMAL;
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
	byte->type = AOUT_TYPE_NORMAL;
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
	byte->type = AOUT_TYPE_NORMAL;
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

struct aout_byte* aout_create_metadata_extension(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_EXTENSION;
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

struct aout_byte* aout_create_metadata_incbin(char* path)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_INCBIN;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
	byte->label = path;
	byte->label_replace = NULL;
	return byte;
}

struct aout_byte* aout_create_metadata_origin(uint16_t address)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_ORIGIN;
	byte->opcode = address;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
	byte->label = NULL;
	byte->label_replace = NULL;
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


void aout_write(FILE* out, bool relocatable)
{
	struct aout_byte* current_outer;
	struct aout_byte* current_inner;
	uint32_t mem_index, i, path_i;
	uint16_t inst;
	uint16_t code_offset = 0;
	FILE* temp = NULL;
	char* cname;

	// Initialize out our extension table.
	code_offset += textn_init(start);

	// If relocatable, initialize out our relocation table.
	if (relocatable)
		code_offset += treloc_init(start);

	// First go through and replace all labels that need to be.
	current_outer = start;
	while (current_outer != NULL)
	{
		if (current_outer->type != AOUT_TYPE_NORMAL)
		{
			current_outer = current_outer->next;
			continue;
		}

		if (current_outer->label_replace != NULL)
		{
			current_inner = start;
			mem_index = code_offset;
			while (current_inner != NULL)
			{
				if (current_inner->type != AOUT_TYPE_NORMAL)
				{
					current_inner = current_inner->next;
					continue;
				}

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
	
	// Write out our extension table.
	textn_write(out);

	// If relocatable, write out our relocation table.
	if (relocatable)
		treloc_write(out);
	
	// Now write to the file.
	current_outer = start;
	while (current_outer != NULL)
	{
		if (current_outer->type == AOUT_TYPE_METADATA_ORIGIN)
		{
			// Adjust origin.
			fseek(out, current_outer->opcode * 2 /* double because the number is in words, not bytes */, SEEK_SET);
		}
		else if (current_outer->type == AOUT_TYPE_METADATA_INCBIN)
		{
			// Include binary file.
			temp = NULL;
			path_i = 0;
			while (temp == NULL && path_i < pp_path_count)
			{
				// Calculate path.
				cname = malloc(strlen(pp_path_names[path_i]) + 1 + strlen(current_outer->label) + 1);
				memset(cname, 0, strlen(pp_path_names[path_i]) + 1 + strlen(current_outer->label) + 1);
				memcpy(cname, pp_path_names[path_i], strlen(pp_path_names[path_i]));
				memcpy(cname + strlen(pp_path_names[path_i]), "/", 1);
				memcpy(cname + strlen(pp_path_names[path_i]) + 1, current_outer->label, strlen(current_outer->label));

				// Attempt open.
				temp = fopen(cname, "rb");
				if (temp == NULL)
					path_i++;
			}
			if (temp == NULL)
				ahalt(ERR_UNABLE_TO_INCBIN, current_outer->label);

			// Copy binary data.
			while (!feof(temp))
			{
				// TODO: This could be faster if we didn't do it character
				// by character.
				fputc(fgetc(temp), out);
			}

			// Finalize.
			fclose(temp);
			free(cname);
		}
		else if (current_outer->type == AOUT_TYPE_NORMAL)
		{
			// Normal output.
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
		}

		// Goto next in linked list.
		current_outer = current_outer->next;
	}

	fflush(out);
}