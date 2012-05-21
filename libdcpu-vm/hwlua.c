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
#include "hw.h"
#include "hwlua.h"
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

struct lua_hardware* vm_hw_lua_extract_hardware(lua_State* L, int idx)
{
	struct lua_hardware* hw = NULL;
	lua_getmetatable(L, idx);
	lua_rawgeti(L, lua_gettop(L), 0);
	hw = (struct lua_hardware*)lua_touserdata(L, -1);
	lua_pop(L, 2);
	return hw;
}

int vm_hw_lua_handle_register_get(lua_State* L)
{
	// Table is at 1, key is at 2.
	struct lua_hardware* hw = vm_hw_lua_extract_hardware(L, 1);
	bstring name = bfromcstr(lua_tostring(L, 2));

	if (biseqcstrcaseless(name, "A"))
		lua_pushnumber(L, hw->vm->registers[REG_A]);
	else if (biseqcstrcaseless(name, "B"))
		lua_pushnumber(L, hw->vm->registers[REG_B]);
	else if (biseqcstrcaseless(name, "C"))
		lua_pushnumber(L, hw->vm->registers[REG_C]);
	else if (biseqcstrcaseless(name, "X"))
		lua_pushnumber(L, hw->vm->registers[REG_X]);
	else if (biseqcstrcaseless(name, "Y"))
		lua_pushnumber(L, hw->vm->registers[REG_Y]);
	else if (biseqcstrcaseless(name, "Z"))
		lua_pushnumber(L, hw->vm->registers[REG_Z]);
	else if (biseqcstrcaseless(name, "I"))
		lua_pushnumber(L, hw->vm->registers[REG_I]);
	else if (biseqcstrcaseless(name, "J"))
		lua_pushnumber(L, hw->vm->registers[REG_J]);
	else if (biseqcstrcaseless(name, "SP"))
		lua_pushnumber(L, hw->vm->sp);
	else if (biseqcstrcaseless(name, "PC"))
		lua_pushnumber(L, hw->vm->pc);
	else if (biseqcstrcaseless(name, "IA"))
		lua_pushnumber(L, hw->vm->ia);
	else if (biseqcstrcaseless(name, "EX"))
		lua_pushnumber(L, hw->vm->ex);
	else
		lua_pushnil(L);

	return 1;
}

int vm_hw_lua_handle_register_set(lua_State* L)
{
	// Table is at 1, key is at 2, value is at 3.
	struct lua_hardware* hw = vm_hw_lua_extract_hardware(L, 1);
	bstring name = bfromcstr(lua_tostring(L, 2));

	if (biseqcstrcaseless(name, "A"))
		hw->vm->registers[REG_A] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "B"))
		hw->vm->registers[REG_B] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "C"))
		hw->vm->registers[REG_C] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "X"))
		hw->vm->registers[REG_X] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "Y"))
		hw->vm->registers[REG_Y] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "Z"))
		hw->vm->registers[REG_Z] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "I"))
		hw->vm->registers[REG_I] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "J"))
		hw->vm->registers[REG_J] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "SP"))
		hw->vm->sp = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "PC"))
		hw->vm->pc = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "IA"))
		hw->vm->ia = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "EX"))
		hw->vm->ex = (uint16_t)lua_tonumber(L, 3);
	else
		lua_pushnil(L);

	return 1;
}

int vm_hw_lua_handle_ram_get(lua_State* L)
{
	// Table is at 1, key is at 2.
	struct lua_hardware* hw = vm_hw_lua_extract_hardware(L, 1);
	if (!lua_isnumber(L, 2))
	{
		lua_pushstring(L, "ram access must be by numeric value (use array operator)");
		lua_error(L);
		return 0;
	}
	lua_pushnumber(L, hw->vm->ram[(uint16_t)lua_tonumber(L, 2)]);
	return 1;
}

int vm_hw_lua_handle_ram_set(lua_State* L)
{
	// Table is at 1, key is at 2, value is at 3.
	struct lua_hardware* hw = vm_hw_lua_extract_hardware(L, 1);
	if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
	{
		lua_pushstring(L, "ram write must be by numeric value (use array operator / numeric value)");
		lua_error(L);
		return 0;
	}
	hw->vm->ram[(uint16_t)lua_tonumber(L, 2)] = (uint16_t)lua_tonumber(L, 3);
	return 0;
}

void vm_hw_lua_push_cpu(vm_t* vm, struct lua_hardware* hw)
{
	int cpu, registers, registers_mt, ram, ram_mt;

	// Create tables.
	lua_newtable(hw->state);
	cpu = lua_gettop(hw->state);
	lua_newtable(hw->state);
	registers = lua_gettop(hw->state);
	lua_newtable(hw->state);
	registers_mt = lua_gettop(hw->state);
	lua_newtable(hw->state);
	ram = lua_gettop(hw->state);
	lua_newtable(hw->state);
	ram_mt = lua_gettop(hw->state);

	// Push userdata into metatables.
	lua_pushlightuserdata(hw->state, hw);
	lua_rawseti(hw->state, ram_mt, 0);
	lua_pushlightuserdata(hw->state, hw);
	lua_rawseti(hw->state, registers_mt, 0);

	// Create the metatable functions.
	lua_pushcfunction(hw->state, vm_hw_lua_handle_ram_get);
	lua_setfield(hw->state, ram_mt, "__index");
	lua_pushcfunction(hw->state, vm_hw_lua_handle_ram_set);
	lua_setfield(hw->state, ram_mt, "__newindex");
	lua_pushcfunction(hw->state, vm_hw_lua_handle_register_get);
	lua_setfield(hw->state, registers_mt, "__index");
	lua_pushcfunction(hw->state, vm_hw_lua_handle_register_set);
	lua_setfield(hw->state, registers_mt, "__newindex");

	// Associate metatables.
	lua_pushvalue(hw->state, ram_mt);
	lua_setmetatable(hw->state, ram);
	lua_pushvalue(hw->state, registers_mt);
	lua_setmetatable(hw->state, registers);

	// FIXME: Protect the metatables.
	//lua_pushboolean(hw->state, true);
	//lua_setfield(hw->state, ram_mt, "__metatable");
	//lua_pushboolean(hw->state, true);
	//lua_setfield(hw->state, registers_mt, "__metatable");

	// Put ram and registers into CPU.
	lua_pushvalue(hw->state, ram);
	lua_setfield(hw->state, cpu, "ram");
	lua_pushvalue(hw->state, registers);
	lua_setfield(hw->state, cpu, "registers");

	// Clean up stack.
	lua_pop(hw->state, lua_gettop(hw->state) - cpu);

	// CPU is now on top of the stack.
}

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
	vm_hw_lua_push_cpu(vm, hw);

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
	vm_hw_lua_push_cpu(vm, hw);
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
	vm_hw_lua_push_cpu(vm, hw);

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
	bstring path;
	struct lua_hardware* hw;
	int module, hwinfo;

	// Calculate full path to file.
	path = osutil_getarg0path();
#ifdef _WIN32
	bcatcstr(path, "modules\\");
#else
	bcatcstr(path, "modules/");
#endif
	bconcat(path, name);

	// Create the new lua hardware structure.
	hw = malloc(sizeof(struct lua_hardware));
	hw->vm = vm;
	hw->state = lua_open();
	assert(hw->state != NULL);
	luaL_openlibs(hw->state);

	// Execute the code in the new Lua context.
	if (luaL_dofile(hw->state, path->data) != 0)
	{
		printf("lua error was %s.\n", lua_tostring(hw->state, -1));

		// Return NULL.
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
		free(hw);
		bdestroy(path);
		return NULL;
	}
	
	// TODO: Check to see whether the module is
	// a hardware module.

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
	hw->cycle_id = vm_hook_register(vm, &vm_hw_lua_cycle, HOOK_ON_CYCLE, hw);
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
	bcatcstr(hwpath, "\\hw");
#else
	bcatcstr(hwpath, "/hw");
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
		name = blk2bstr(&entry->d_name, entry->d_reclen);

		// Check to see whether it is a lua file.
		if (!binstrr(name, blength(name) - 4, ext))
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
