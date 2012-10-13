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
#include <bfile.h>
#include <iio.h>
#include <pp.h>
#include <ppfind.h>
#include <assert.h>
#include <ddata.h>
#include <debug.h>
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);

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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
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
	list_init(&byte->symbols);
	return byte;
}

struct aout_byte* aout_create_metadata_section(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_SECTION;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
	list_init(&byte->symbols);
	return byte;
}

struct aout_byte* aout_create_metadata_output(char* name)
{
	struct aout_byte* byte = malloc(sizeof(struct aout_byte));
	byte->type = AOUT_TYPE_METADATA_OUTPUT;
	byte->opcode = 0;
	byte->a = 0;
	byte->b = 0;
	byte->next = NULL;
	byte->prev = NULL;
	byte->expr = NULL;
	byte->label = name;
	byte->raw_used = false;
	byte->raw = 0x0;
	list_init(&byte->symbols);
	return byte;
}

struct aout_byte* aout_emit(struct aout_byte* byte)
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

	return byte;
}

// Writing state variables.
uint16_t code_offset = 0;
uint16_t true_origin = 0;

uint16_t aout_get_label_address(bstring name)
{
	struct aout_byte* current = start;
	uint16_t mem_index = code_offset;
	bool found_import = false;

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
				// Set a temporary variable in case we
				// define the label later (in which case we'll
				// use that instead of throwing an error).
				found_import = true;
			}
		}

		// Goto next.
		current = current->next;
	}

	// Error if we get to here.
	if (found_import)
		ahalt(ERR_EXPRESSION_NOT_PERMITTED, name->data);
	else
		ahalt(ERR_LABEL_NOT_FOUND, name->data);
	return 0;
}

uint16_t aout_write(FILE* out, bool relocatable, bool intermediate)
{
	struct aout_byte* current_outer;
	struct aout_byte* current_inner;
	struct lprov_entry* linker_provided = NULL;
	struct lprov_entry* linker_required = NULL;
	struct lprov_entry* linker_adjustment = NULL;
	struct lprov_entry* linker_section = NULL;
	struct lprov_entry* linker_output = NULL;
	struct lprov_entry* linker_temp = NULL;
	uint32_t mem_index, out_index;
	uint16_t inst;
	BFILE* temp = NULL;
	bstring bname, ename;
	uint16_t eaddr;
	bool did_find;
	bool shown_expr_warning = false;
	bool has_output = false;

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
		else if (current_outer->type == AOUT_TYPE_METADATA_SECTION)
		{
			assert(current_outer->label != NULL);

			// We're exporting the current address as the beginning
			// of a section.
			if (!intermediate)
				ahalt(ERR_NOT_GENERATING_INTERMEDIATE_CODE, NULL);

			// Check to make sure outputs haven't previously been emitted.
			if (has_output)
				ahalt(ERR_OUTPUT_BEFORE_SECTION, NULL);

			// Create linker entry.
			linker_temp = lprov_create(current_outer->label, out_index);
			linker_temp->next = linker_section;
			linker_section = linker_temp;
			printd(LEVEL_VERBOSE, "LINK SECTION %s -> 0x%04X\n", current_outer->label, out_index);
		}
		else if (current_outer->type == AOUT_TYPE_METADATA_OUTPUT)
		{
			assert(current_outer->label != NULL);

			// We're exporting the current address as the beginning
			// of a section.
			if (!intermediate)
				ahalt(ERR_NOT_GENERATING_INTERMEDIATE_CODE, NULL);

			// Create linker entry.
			has_output = true;
			linker_temp = lprov_create(current_outer->label, out_index);
			linker_temp->next = linker_output;
			linker_output = linker_temp;
			printd(LEVEL_VERBOSE, "LINK OUTPUT 0x%04X -> %s\n", out_index, current_outer->label);
		}
		else if (current_outer->type == AOUT_TYPE_METADATA_EXPORT)
		{
			assert(current_outer->label != NULL);

			// We're exporting the address of this label in the
			// object table.
			if (!intermediate)
				ahalt(ERR_NOT_GENERATING_INTERMEDIATE_CODE, NULL);

			// Resolve label position.
			ename = bfromcstr(current_outer->label);
			eaddr = aout_get_label_address(ename);
			bdestroy(ename);

			// Create linker entry.
			linker_temp = lprov_create(current_outer->label, eaddr);
			linker_temp->next = linker_provided;
			linker_provided = linker_temp;
			printd(LEVEL_VERBOSE, "LINK REPLACE %s -> 0x%04X\n", current_outer->label, eaddr);
		}
		else if (current_outer->type == AOUT_TYPE_NORMAL && current_outer->expr != NULL)
		{
			if (current_outer->expr->type != EXPR_LABEL)
			{
				// This is either just a number or a more complicated expression, so
				// evaluate it using the preprocessor expression engine.
				if ((relocatable || intermediate) && !shown_expr_warning)
				{
					printd(LEVEL_WARNING, "warning: expressions will not be adjusted at link or relocation time.\n");
					printd(LEVEL_WARNING, "		ensure labels are not used as part of expressions.\n");
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
					else if (current_inner->type == AOUT_TYPE_METADATA_IMPORT)
					{
						// An imported label (we don't need to adjust
						// memory index because the existance of this type
						// of entry doesn't affect executable size).
						if (!intermediate)
							ahalt(ERR_NOT_GENERATING_INTERMEDIATE_CODE, NULL);

						assert(current_outer->expr->data != NULL);
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
							printd(LEVEL_VERBOSE, "LINK REPLACE 0x%04X -> %s\n", out_index, current_inner->label);
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

					// We also need to add this entry to the adjustment
					// table for the linker since it also needs to adjust
					// internal label jumps in files when it concatenates
					// all of the object code together.
					linker_temp = lprov_create(NULL, out_index);
					linker_temp->next = linker_adjustment;
					linker_adjustment = linker_temp;
					printd(LEVEL_VERBOSE, "LINK ADJUST 0x%04X\n", out_index);
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
		objfile_save(out, linker_provided, linker_required, linker_adjustment, linker_section, linker_output);

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
			fseek(out, true_origin + current_outer->opcode * 2, SEEK_SET);
		}
		else if (current_outer->type == AOUT_TYPE_METADATA_INCBIN)
		{
			// Include binary file.
			bname = ppfind_locate(bautofree(bfromcstr(current_outer->label)));

			if (bname == NULL)
				ahalt(ERR_UNABLE_TO_INCBIN, current_outer->label);

			temp = bfopen((const char*)(bname->data), "rb");

			if (temp == NULL)
				ahalt(ERR_UNABLE_TO_INCBIN, current_outer->label);

			// Copy binary data.
			while (!bfeof(temp))
			{
				// TODO: This could be faster if we didn't do it character
				// by character.
				fputc(bfgetc(temp), out);
			}

			// Finalize.
			bfclose(temp);
			bdestroy(bname);
		}
		else if (current_outer->type == AOUT_TYPE_NORMAL)
		{
			// Update the debugging symbol.
			dbgfmt_update_symbol_list(&current_outer->symbols, (uint16_t)((ftell(out) - true_origin) / 2));

			// Normal output.
			if (current_outer->raw_used == true)
			{
				inst = current_outer->raw;
				iwrite(&inst, out);
			}
			else if (current_outer->label == NULL)
			{
				inst = INSTRUCTION_CREATE(current_outer->opcode, current_outer->a, current_outer->b);
				iwrite(&inst, out);
			}
		}

		// Goto next in linked list.
		current_outer = current_outer->next;
	}

	fflush(out);

	return (uint16_t)((ftell(out) - true_origin) / 2);
}
