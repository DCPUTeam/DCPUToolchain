/**

	File:		pplua.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-PP

	Authors:	James Rhodes

	Description:	Declares the public API for the preprocessor to load
			custom Lua modules.

**/

#ifndef __DCPU_LIBDCPU_PP_LUA_H
#define __DCPU_LIBDCPU_PP_LUA_H

#include <bstring.h>
#include "dcpu.h"

struct pp_state
{
	void (*pp_lua_print)(const char* text, void* ud);
	void (*pp_lua_print_line)(const char* text, void* ud);
	void (*pp_lua_scope_enter)(bool output, void* ud);
	void (*pp_lua_scope_exit)(void* ud);
	void (*pp_lua_add_symbol)(const char* symbol, void* ud);
};

struct customarg_entry
{
	struct expr* expr;
	bstring word;
	bstring string;
	int number;
};

void pp_lua_init();
void pp_lua_handle(struct pp_state* state, void* scanner, bstring name, list_t* parameters);

#endif