/**

	File:		ldconv.c

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Converts object format structures to linked lists
			so that they can be easily modified and cloned.

**/

#include <stdlib.h>
#include <bstring.h>
#include <simclist.h>
#include <lprov.h>
#include "ldconv.h"

size_t lconv_entry_meter(const void* el)
{
	return sizeof(struct lconv_entry);
}

int lconv_entry_comparator(const void* a, const void* b)
{
	struct lconv_entry* ea = (struct lconv_entry*)a;
	struct lconv_entry* eb = (struct lconv_entry*)b;
	if (ea->address > eb->address)
		return -1;
	else if (ea->address == eb->address)
		return 0;
	else
		return 1;
}

list_t* list_create()
{
	list_t* list = malloc(sizeof(list_t));
	list_init(list);
	list_attributes_copy(list, &lconv_entry_meter, 1);
	list_attributes_comparator(list, &lconv_entry_comparator);
	return list;
}

list_t* list_convert(struct lprov_entry* first)
{
	struct lconv_entry* entry;
	list_t* list = malloc(sizeof(list_t));
	list_init(list);
	list_attributes_copy(list, &lconv_entry_meter, 1);
	list_attributes_comparator(list, &lconv_entry_comparator);
	while (first != NULL)
	{
		entry = malloc(sizeof(struct lconv_entry));
		entry->address = first->address;
		entry->label = bfromcstr(first->label);
		list_append(list, entry);
		free(entry);
		first = first->next;
	}
	return list;
}

list_t* list_clone(list_t* original)
{
	list_t* list = malloc(sizeof(list_t));
	list_init(list);
	list_attributes_copy(list, &lconv_entry_meter, 1);
	list_attributes_comparator(list, &lconv_entry_comparator);
	list_iterator_start(original);
	while (list_iterator_hasnext(original))
		list_append(list, list_iterator_next(original));
	list_iterator_stop(original);
	return list;
}