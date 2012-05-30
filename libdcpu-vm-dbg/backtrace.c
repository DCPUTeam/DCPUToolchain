/**

	File:		breakpoint.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM-DBG

	Authors:	James Rhodes

	Description:	Defines breakpoint functionality.

**/

#include <stdlib.h>
#include <simclist.h>
#include <dcpu.h>
#include "backtrace.h"

size_t backtrace_entry_meter(const void* el)
{
	return sizeof(struct backtrace_entry);
}

struct backtrace_entry* backtrace_entry_create(uint16_t caller, uint16_t callee)
{
	struct backtrace_entry* en = malloc(sizeof(struct backtrace_entry));
	en->caller = caller;
	en->callee = callee;
	return en;
}

list_t backtrace_list_create()
{
	list_t list;
	list_init(&list);
	list_attributes_copy(&list, &backtrace_entry_meter, 1);
	return list;
}

