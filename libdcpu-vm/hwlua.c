/**

	File:		hwlua.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-VM

	Authors:	James Rhodes

	Description:	Hosts custom Lua-based hardware.

**/

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <bstring.h>
#include <simclist.h>
#include <dirent.portable.h>
#include <osutil.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>
#include <ppexprlua.h>
#include "hw.h"
#include "hwlua.h"
#include "hwluacpu.h"
#include "dcpuhook.h"
#include "dcpubase.h"
#include "dcpuops.h"

bool vm_lua_online;
list_t vm_hardware;

struct lua_hardware
{
	vm_t* vm;
	hw_t device;
	lua_State* state;
	uint16_t cycle_id;
	uint16_t write_id;
};

void vm_hw_lua_cycle(vm_t* vm, uint16_t pos, void* ud)
{
	struct lua_hardware* hw = (struct lua_hardware*)ud;
	int cycle;
	lua_getglobal(hw->state, "cycle");
	cycle = lua_gettop(hw->state);
	if (lua_isnoneornil(hw->state, -1))
	{
		lua_pop(hw->state, 1);
		return;
	}

	// Load function and arguments.
	lua_pushvalue(hw->state, cycle);
	vm_hw_lua_cpu_push_cpu(hw->state, vm);

	// Call the function.
	if (lua_pcall(hw->state, 1, 0, 0) != 0)
	{
		printf("lua error in cycle was %s.\n", lua_tostring(hw->state, -1));
		lua_pop(hw->state, 1);
	}
	lua_pop(hw->state, 1);
}

void vm_hw_lua_write(vm_t* vm, uint16_t pos, void* ud)
{
	struct lua_hardware* hw = (struct lua_hardware*)ud;
	int write;
	lua_getglobal(hw->state, "write");
	write = lua_gettop(hw->state);
	if (lua_isnoneornil(hw->state, write))
	{
		lua_pop(hw->state, 1);
		return;
	}

	// Load function and arguments.
	lua_pushvalue(hw->state, write);
	vm_hw_lua_cpu_push_cpu(hw->state, vm);
	lua_pushnumber(hw->state, pos);

	// Call the function.
	if (lua_pcall(hw->state, 2, 0, 0) != 0)
	{
		printf("lua error in write was %s.\n", lua_tostring(hw->state, -1));
		lua_pop(hw->state, 1);
	}
	lua_pop(hw->state, 1);
}

void vm_hw_lua_interrupt(vm_t* vm, void* ud)
{
	struct lua_hardware* hw = (struct lua_hardware*)ud;
	int interrupt;
	lua_getglobal(hw->state, "interrupt");
	interrupt = lua_gettop(hw->state);
	if (lua_isnoneornil(hw->state, interrupt))
	{
		lua_pop(hw->state, 1);
		return;
	}

	// Load function and arguments.
	lua_pushvalue(hw->state, interrupt);
	vm_hw_lua_cpu_push_cpu(hw->state, vm);

	// Call the function.
	if (lua_pcall(hw->state, 1, 0, 0) != 0)
	{
		printf("lua error in interrupt was %s.\n", lua_tostring(hw->state, -1));
		lua_pop(hw->state, 1);
	}
	lua_pop(hw->state, 1);
}

struct lua_hardware* vm_hw_lua_load(vm_t* vm, bstring name)
{
	bstring path, modtype;
	struct lua_hardware* hw;
	int module, hwinfo;

	// Calculate full path to file.
	path = osutil_getarg0path();
#ifdef _WIN32
	bcatcstr(path, "modules\\");
#else
	bcatcstr(path, "/modules/");
#endif
	bconcat(path, name);

	// Create the new lua hardware structure.
	hw = malloc(sizeof(struct lua_hardware));
	hw->vm = vm;
	hw->state = lua_open();
	assert(hw->state != NULL);
	luaL_openlibs(hw->state);
	luaX_loadexpressionlib(hw->state);

	// Execute the code in the new Lua context.
	if (luaL_dofile(hw->state, path->data) != 0)
	{
		printf("lua error was %s.\n", lua_tostring(hw->state, -1));

		// Return NULL.
		lua_close(hw->state);
		free(hw);
		bdestroy(path);
		return NULL;
	}

	// Load tables.
	lua_getglobal(hw->state, "MODULE");
	module = lua_gettop(hw->state);
	lua_getglobal(hw->state, "HARDWARE");
	hwinfo = lua_gettop(hw->state);

	// Ensure both tables were provided.
	if (lua_isnoneornil(hw->state, module) || lua_isnoneornil(hw->state, hwinfo))
	{
		printf("failed to load hardware from %s.\n", path->data);

		// Return NULL.
		lua_close(hw->state);
		free(hw);
		bdestroy(path);
		return NULL;
	}

	// Check to see whether the module is
	// a hardware module.
	lua_getfield(hw->state, module, "Type");
	modtype = bfromcstr(lua_tostring(hw->state, -1));
	if (!biseqcstrcaseless(modtype, "Hardware"))
	{
		// Return NULL.
		lua_pop(hw->state, 1);
		lua_close(hw->state);
		free(hw);
		bdestroy(modtype);
		bdestroy(path);
		return NULL;
	}
	lua_pop(hw->state, 1);
	bdestroy(modtype);

	// Store information into the Lua
	// hardware structure.
	lua_getfield(hw->state, hwinfo, "ID");
	lua_getfield(hw->state, hwinfo, "Version");
	lua_getfield(hw->state, hwinfo, "Manufacturer");
	hw->device.id = (uint32_t)lua_tonumber(hw->state, lua_gettop(hw->state) - 2);
	hw->device.version = (uint16_t)lua_tonumber(hw->state, lua_gettop(hw->state) - 1);
	hw->device.manufacturer = (uint32_t)lua_tonumber(hw->state, lua_gettop(hw->state));
	lua_pop(hw->state, 3);

	printf("hardware loaded ID: %u, Version: %u, Manufacturer: %u.\n", hw->device.id, hw->device.version, hw->device.manufacturer);

	// Register the hardware.
	hw->device.handler = &vm_hw_lua_interrupt;
	hw->device.userdata = hw;
	vm_hw_register(vm, hw->device);

	// Register the hooks.
	hw->cycle_id = vm_hook_register(vm, &vm_hw_lua_cycle, HOOK_ON_POST_CYCLE, hw);
	hw->write_id = vm_hook_register(vm, &vm_hw_lua_write, HOOK_ON_WRITE, hw);

	// Pop tables from stack.
	lua_pop(hw->state, 2);

	// Return new hardware.
	return hw;
}

void vm_hw_lua_unload(struct lua_hardware* hw)
{

}

void vm_hw_lua_init(vm_t* vm)
{
	DIR* dir;
	struct dirent* entry;
	bstring name = NULL;
	bstring hwpath = NULL;
	bstring ext = bfromcstr(".lua");

	// Work out the name of the hw directory.
	hwpath = osutil_getarg0path();
#ifdef _WIN32
	bcatcstr(hwpath, "\\modules");
#else
	bcatcstr(hwpath, "/modules");
#endif

	// Attempt to open the hw directory.
	dir = opendir(hwpath->data);
	if (dir == NULL)
	{
		// The directory does not exist, so we don't load
		// any custom hardware.
		vm_lua_online = false;
		bdestroy(hwpath);
		bdestroy(ext);
		return;
	}

	// Load each file from the hw directory.
	while ((entry = readdir(dir)) != NULL)
	{
		name = bfromcstr(&entry->d_name[0]);

		// Check to see whether it is a lua file.
		if (binstr(name, blength(name) - 4, ext) == BSTR_ERR)
		{
			// Not a Lua file, skip over and
			// then continue.
			bdestroy(name);
			continue;
		}

		// Check to see if it is a normal file.
		if (entry->d_type != DT_REG)
		{
			// Not a file, skip over and then
			// continue.
			bdestroy(name);
			continue;
		}

		// Attempt to load the Lua file.
		printf("loading Lua hardware from: %s\n", name->data);
		vm_hw_lua_load(vm, name);

		// Free data.
		bdestroy(name);
	}

	// Free resources.
	closedir(dir);

	/*


	hw_t keyboard;

	keyboard.id = 0x30CF7406;
	keyboard.version = 0x1802;
	keyboard.manufacturer = 0x1C6C8B36;
	keyboard.handler = &vm_hw_lua_interrupt;

	// Register hooks.
	vm_cycle_update = vm_hook_register(vm, &vm_hw_lua_cycle, HOOK_ON_CYCLE);
	vm_write_update = vm_hook_register(vm, &vm_hw_lua_write, HOOK_ON_WRITE);
	vm_hw_register(vm, keyboard);*/
}

void vm_hw_lua_free(vm_t* vm)
{
	// Unregister hooks.
	//vm_hook_unregister(vm, vm_cycle_update);
	//vm_hook_unregister(vm, vm_write_update);
}
