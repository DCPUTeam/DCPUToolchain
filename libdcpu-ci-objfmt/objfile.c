/**

	File:			objfile.c

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Defines functions used for reading and writing
					to object files.

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldata.h"
#include "lprov.h"
#include "objfile.h"

void objfile_load(const char* filename, FILE* in, uint16_t* offset, struct lprov_entry** provided, struct lprov_entry** required, struct lprov_entry** adjustment)
{
	struct ldata_entry* entry = NULL;
	struct lprov_entry* prov_last = provided == NULL ? NULL : *provided;
	struct lprov_entry* prov_current = NULL;
	struct lprov_entry* req_last = required == NULL ? NULL : *required;
	struct lprov_entry* req_current = NULL;
	struct lprov_entry* adjust_last = adjustment == NULL ? NULL : *adjustment;
	struct lprov_entry* adjust_current = NULL;
	size_t sz;

	// Read <256 byte label content> <mode> <address>
	// until mode is LABEL_END.
	//
	// If address is LABEL_REQUIRED, then it's a label that
	// needs resolving, otherwise it's a label that is
	// provided to other object files.
	entry = ldata_read(in);
	while (entry->mode != LABEL_END)
	{
		if (entry->mode == LABEL_PROVIDED && provided != NULL)
		{
			prov_current = lprov_create(entry->label, entry->address + *offset);
			if (prov_last == NULL)
				*provided = prov_current;
			else
				prov_last->next = prov_current;
			prov_last = prov_current;
		}
		else if (entry->mode == LABEL_REQUIRED && required != NULL)
		{
			req_current = lprov_create(entry->label, entry->address + *offset);
			if (req_last == NULL)
				*required = req_current;
			else
				req_last->next = req_current;
			req_last = req_current;
		}
		else if (entry->mode == LABEL_ADJUSTMENT && adjustment != NULL)
		{
			adjust_current = lprov_create(NULL, entry->address + *offset);
			if (adjust_last == NULL)
				*adjustment = adjust_current;
			else
				adjust_last->next = adjust_current;
			adjust_last = adjust_current;
		}

		entry = ldata_read(in);
	}

	// Now read the rest of the file to determine it's total
	// length so that we can adjust what will be the offset.
	sz = ftell(in);
	fseek(in, 0, SEEK_END);
	*offset += (uint16_t)((ftell(in) - sz) / 2); // Divide by two since these values are in bytes and we need the offset in words.
	fseek(in, sz, SEEK_SET);
}

void objfile_save(FILE* out, struct lprov_entry* provided, struct lprov_entry* required, struct lprov_entry* adjustment)
{
	struct ldata_entry* entry = NULL;

	// First write out the provided table.
	while (provided != NULL)
	{
		entry = malloc(sizeof(struct ldata_entry));
		entry->mode = LABEL_PROVIDED;
		entry->address = provided->address;
		strcpy(entry->label, provided->label);
		ldata_write(out, entry);

		provided = provided->next;
	}

	// Now write out the required table.
	while (required != NULL)
	{
		entry = malloc(sizeof(struct ldata_entry));
		entry->mode = LABEL_REQUIRED;
		entry->address = required->address;
		strcpy(entry->label, required->label);
		ldata_write(out, entry);

		required = required->next;
	}

	// Now write out the adjustment table.
	while (adjustment != NULL)
	{
		entry = malloc(sizeof(struct ldata_entry));
		entry->mode = LABEL_ADJUSTMENT;
		entry->address = adjustment->address;
		memset(entry->label, 0, 256);
		ldata_write(out, entry);

		adjustment = adjustment->next;
	}

	// Now write out the NULL entry.
	entry = malloc(sizeof(struct ldata_entry));
	entry->mode = LABEL_END;
	entry->address = 0;
	memset(entry->label, 0, 256);
	ldata_write(out, entry);
}
