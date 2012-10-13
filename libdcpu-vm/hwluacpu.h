/**

	File:		hwlua.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-VM

	Authors:	James Rhodes

	Description:	Declares functions used to
			host custom Lua hardware.

**/

#ifndef __DCPU_HWLUACPU_H
#define __DCPU_HWLUACPU_H

#include <lua.h>
#include "dcpu.h"

void vm_hw_lua_cpu_push_cpu(lua_State* L, vm_t* vm);

#endif
