/**

	File:		ldlua.h

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides optimization infrastructure for Lua-based
			optimizers.

**/

#ifndef __DCPU_LD_LUA_H
#define __DCPU_LD_LUA_H

#include "ldbin.h"

void bin_lua_init();
int32_t bin_lua_optimize(struct ldbin* bin);

#endif