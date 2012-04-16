/**

	File:			lprov.c

	Project:		DCPU-16 Tools
	Component:		Linker

	Authors:		James Rhodes

	Description:	Defines functions used in the provided label
					linked-list.

**/

#include <stdlib.h>
#include "lprov.h"

struct lprov_entry* lprov_create(char* label, uint16_t address)
{
	struct lprov_entry* entry = malloc(sizeof(struct lprov_entry));
	entry->label = label;
	entry->address = address;
	entry->next = NULL;
	return entry;
}