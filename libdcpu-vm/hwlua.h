/**

	File:		hwlua.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-VM

	Authors:	James Rhodes

	Description:	Declares functions used to
			host custom Lua hardware.

**/

#ifndef __DCPUHWLUA_H
#define __DCPUHWLUA_H

#include "dcpu.h"

void vm_hw_lua_init(vm_t* vm);
void vm_hw_lua_free(vm_t* vm);

#endif
