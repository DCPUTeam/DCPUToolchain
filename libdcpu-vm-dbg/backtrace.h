/**

	File:		breakpoint.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM-DBG

	Authors:	James Rhodes

	Description:	Declares breakpoint structures.

**/

#ifndef _DCPU_DBG_BACKTRACE
#define _DCPU_DBG_BACKTRACE

#include <simclist.h>
#include <dcpu.h>

struct backtrace_entry
{
	uint16_t caller;
	uint16_t callee;
};

struct backtrace_entry* backtrace_entry_create(uint16_t caller, uint16_t callee);
list_t backtrace_list_create();

#endif
