/**

	File:		aout.c

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Implements outputting emitted opcodes and raw
			values into RAM images.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <bstring.h>
#include <pp.h>
#include <ppfind.h>
#include "aout.h"
#include "aerr.h"
#include "dcpu.h"
#include "treloc.h"
#include "textn.h"
#include "ldata.h"
#include "lprov.h"
#include "objfile.h"

struct aout_byte* start = NULL;
struct aout_byte* end = NULL;
extern char* fileloc;

struct aout_byte* aout_create_raw(uint16_t raw)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_NORMAL;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = NULL;
	byte->raw_used = true;
	byte->raw = raw;
	return byte;
}

struct aout_byte* aout_create_opcode(uint16_t opcode, uint16_t a, uint16_t b)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_NORMAL;
	byte->opcode = opcode;
	byte->a = a;
	byte->b = b;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;

	if (opcode == 0 && a == 0)
		ahalt(ERR_OUTPUT_NULL, NULL);

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
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
	return byte;
}

struct aout_byte* aout_create_expr(struct expr* expression)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_NORMAL;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = expression;
	byte->label = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
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
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
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
	byte->expr = NULL;
	byte->label = path;
	byte->raw_used = false;
	byte->raw = 0x0;
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
	byte->expr = expr_new_number(address);
	byte->label = NULL;
	byte->raw_used = false;
	byte->raw = 0x0;
	return byte;
}

struct aout_byte* aout_create_metadata_export(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_EXPORT;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
	return byte;
}

struct aout_byte* aout_create_metadata_import(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_IMPORT;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
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

// Writing state variables.
uint16_t code_offset = 0;
uint16_t true_origin = 0;

uint16_t aout_get_label_address(bstring name)
{
	struct aout_byte* current = start;
	uint16_t mem_index = code_offset;

	while (current != NULL)
	{
		if (current->type == AOUT_TYPE_METADATA_ORIGIN)
		{
			// Adjust memory address.
			mem_index = current->opcode;
		}
		else if (current->type == AOUT_TYPE_NORMAL)
		{
			// Replace the label position.
			if (current->label == NULL)
				mem_index += 1;
			else if (strcmp(current->label, name->data) == 0)
			{
				// Return the address of this label.
				return mem_index;
			}
		}
		else if (current->type == AOUT_TYPE_METADATA_IMPORT)
		{
			// Check to see if the requested label is
			// actually imported.
			if (strcmp(current->label, name->data) == 0)
			{
				ahalt(ERR_EXPRESSION_NOT_PERMITTED, name->data);
				return 0;
			}
		}

		// Goto next.
		current = current->next;
	}

	// Error if we get to here.
	ahalt(ERR_LABEL_NOT_FOUND, name->data);
	return 0;
}

void aout_write(FILE* out, bool relocatable, bool intermediate)
{
	struct aout_byte* current_outer;
	struct aout_byte* current_inner;
	struct lprov_entry* linker_provided = NULL;
	struct lprov_entry* linker_required = NULL;
	struct lprov_entry* linker_adjustment = NULL;
	struct lprov_entry* linker_temp = NULL;
	uint32_t mem_index, out_index;
	uint16_t inst;
	FILE* temp = NULL;
	bstring bname;
	bool did_find;
	bool shown_expr_warning = false;

	// Initialize out our extension table.
	code_offset += textn_init(start);

	// If relocatable, initialize out our relocation table.
	if (relocatable)
		code_offset += treloc_init(start);

	// First go through and evaluate all expressions that need to be.
	current_outer = start;
	out_index = code_offset;
	while (current_outer != NULL)
	{
		if (current_outer->type == AOUT_TYPE_METADATA_ORIGIN)
		{
			// Adjust memory address.
			out_index = current_outer->opcode;
		}
		else if (current_outer->type == AOUT_TYPE_NORMAL && current_outer->expr != NULL)
		{
			if (current_outer->expr->type != EXPR_LABEL)
			{
				// This is either just a number or a more complicated expression, so
				// evaluate it using the preprocessor expression engine.
				if ((relocatable || intermediate) && !shown_expr_warning)
				{
					fprintf(stderr, "warning: expressions will not be adjusted at link or relocation time.\n");
					fprintf(stderr, "	  ensure labels are not used as part of expressions.\n");
					shown_expr_warning = true;
				}
				current_outer->raw_used = true;
				current_outer->raw = expr_evaluate(current_outer->expr, &aout_get_label_address, &ahalt_expression_exit_handler);
				expr_delete(current_outer->expr);
				current_outer->expr = NULL;
			}
			else
			{
				// If this is just a label, we can handle it directly (this allows
				// us to handle imported labels in intermediate code).
				current_inner = start;
				mem_index = code_offset;
				did_find = false;

				// Search for .IMPORT directives first.
				while (current_inner != NULL)
				{
					if (current_inner->type == AOUT_TYPE_METADATA_ORIGIN)
					{
						// Adjust memory address.
						mem_index = current_inner->opcode;
					}
					else if (current_inner->type == AOUT_TYPE_METADATA_IMPORT && current_outer->type == AOUT_TYPE_NORMAL)
					{
						// An imported label (we don't need to adjust
						// memory index because the existance of this type
						// of entry doesn't affect executable size).
						if (!intermediate)
							ahalt(ERR_NOT_GENERATING_INTERMEDIATE_CODE, NULL);

						if (strcmp(current_inner->label, ((bstring)current_outer->expr->data)->data) == 0)
						{
							// We don't actually know our position yet;
							// that will be handled by the linker!
							current_outer->raw = 0xFFFF;
							expr_delete(current_outer->expr);
							current_outer->expr = NULL;
							linker_temp = lprov_create(current_inner->label, out_index);
							linker_temp->next = linker_required;
							linker_required = linker_temp;
							fprintf(stderr, "LINK REPLACE 0x%04X -> %s\n", out_index, current_inner->label);
							did_find = true;
							break;
						}
					}

					// Goto next.
					current_inner = current_inner->next;
				}

				// If it wasn't found in the .IMPORT directives, try searching
				// labels directly using the expression engine.
				if (!did_find)
				{
					// Replace the label position.
					current_outer->raw_used = true;
					current_outer->raw = expr_evaluate(current_outer->expr, &aout_get_label_address, &ahalt_expression_exit_handler);
					expr_delete(current_outer->expr);
					current_outer->expr = NULL;
					did_find = true;
				}
			}
		}

		if (current_outer->type == AOUT_TYPE_NORMAL && current_outer->label == NULL)
			out_index += 1;

		current_outer = current_outer->next;
	}

	// If intermediate, we need to write out our linker table
	// as the absolute first thing in the file.
	if (intermediate)
	{
		fwrite(ldata_objfmt, 1, strlen(ldata_objfmt) + 1, out);
		objfile_save(out, linker_provided, linker_required, linker_adjustment);

		// Adjust the "true origin" for .ORIGIN directivies because
		// the linker table won't exist in the final result when
		// linked.
		true_origin = (uint16_t)ftell(out);
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
			fseek(out, true_origin + current_outer->opcode * 2 /* double because the number is in words, not bytes */, SEEK_SET);
		}
		else if (current_outer->type == AOUT_TYPE_METADATA_INCBIN)
		{
			// Include binary file.
			bname = ppfind_locate(bautofree(bfromcstr(current_outer->label)));

			if (bname == NULL)
				ahalt(ERR_UNABLE_TO_INCBIN, current_outer->label);

			temp = fopen((const char*)(bname->data), "rb");

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
			bdestroy(bname);
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
