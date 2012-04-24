/**

	File:			lprov.c

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Defines functions used in the provided label
					linked-list.

**/

#include <stdlib.h>
#include <string.h>
#include "lprov.h"

struct lprov_entry* lprov_create(char* label, uint16_t address)
{
	struct lprov_entry* entry = malloc(sizeof(struct lprov_entry));
	entry->label = label;
	entry->address = address;
	entry->next = NULL;
	return entry;
}

struct lprov_entry* lprov_find_by_label(struct lprov_entry* first, char* label)
{
	while (first != NULL)
	{
		if (strcmp(first->label, label) == 0)
			return first;
		first = first->next;
	}
	return NULL;
}

struct lprov_entry* lprov_find_by_address(struct lprov_entry* first, uint16_t address)
{
	while (first != NULL)
	{
		if (first->address == address)
			return first;
		first = first->next;
	}
	return NULL;
}