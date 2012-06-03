/**

	File:		pplua.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-PP

	Authors:	James Rhodes

	Description:	Declares the public API for the preprocessor to load
			custom Lua modules.

**/

#ifndef __DCPU_LIBDCPU_VM_DBG_LUA_H
#define __DCPU_LIBDCPU_VM_DBG_LUA_H

#include <simclist.h>
#include <bstring.h>
#include "dcpu.h"

struct dbg_state
{
	vm_t* (*get_vm)();
	void (*dbg_lua_break)();
	void (*dbg_lua_run)();
	list_t* (*dbg_lua_get_symbols)();
};

#define DBG_CUSTOMARG_TYPE_PATH 0
#define DBG_CUSTOMARG_TYPE_PARAM 1
#define DBG_CUSTOMARG_TYPE_STRING 2
#define DBG_CUSTOMARG_TYPE_NUMBER 3

struct customarg_entry
{
	int type;
	bstring path;
	bstring param;
	bstring string;
	int number;
};

extern struct dbg_state lstate;

void dbg_lua_init();
void dbg_lua_handle_command(struct dbg_state* state, void* ud, freed_bstring name, list_t* parameters);
void dbg_lua_handle_hook(struct dbg_state* state, void* ud, freed_bstring type);
void dbg_lua_handle_hook_symbol(struct dbg_state* state, void* ud, freed_bstring symbol);

#endif