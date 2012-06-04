/**

	File:		hwluacpu.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes

	Description:	Provides generic functions for exposing the VM's CPU
			state in Lua.

**/

#include <lua.h>
#include <lauxlib.h>
#include <bstring.h>
#include <dcpu.h>
#include "dcpubase.h"
#include "dcpudis.h"
#include "hwluacpu.h"

vm_t* vm_hw_lua_cpu_extract_cpu(lua_State* L, int idx)
{
	vm_t* vm = NULL;
	lua_getmetatable(L, idx);
	lua_rawgeti(L, lua_gettop(L), 0);
	vm = (vm_t*)lua_touserdata(L, -1);
	lua_pop(L, 2);
	return vm;
}

int vm_hw_lua_cpu_handle_register_get(lua_State* L)
{
	// Table is at 1, key is at 2.
	vm_t* vm = vm_hw_lua_cpu_extract_cpu(L, 1);
	bstring name = bfromcstr(lua_tostring(L, 2));

	if (biseqcstrcaseless(name, "A"))
		lua_pushnumber(L, vm->registers[REG_A]);
	else if (biseqcstrcaseless(name, "B"))
		lua_pushnumber(L, vm->registers[REG_B]);
	else if (biseqcstrcaseless(name, "C"))
		lua_pushnumber(L, vm->registers[REG_C]);
	else if (biseqcstrcaseless(name, "X"))
		lua_pushnumber(L, vm->registers[REG_X]);
	else if (biseqcstrcaseless(name, "Y"))
		lua_pushnumber(L, vm->registers[REG_Y]);
	else if (biseqcstrcaseless(name, "Z"))
		lua_pushnumber(L, vm->registers[REG_Z]);
	else if (biseqcstrcaseless(name, "I"))
		lua_pushnumber(L, vm->registers[REG_I]);
	else if (biseqcstrcaseless(name, "J"))
		lua_pushnumber(L, vm->registers[REG_J]);
	else if (biseqcstrcaseless(name, "SP"))
		lua_pushnumber(L, vm->sp);
	else if (biseqcstrcaseless(name, "PC"))
		lua_pushnumber(L, vm->pc);
	else if (biseqcstrcaseless(name, "IA"))
		lua_pushnumber(L, vm->ia);
	else if (biseqcstrcaseless(name, "EX"))
		lua_pushnumber(L, vm->ex);
	else
		lua_pushnil(L);

	return 1;
}

int vm_hw_lua_cpu_handle_register_set(lua_State* L)
{
	// Table is at 1, key is at 2, value is at 3.
	vm_t* vm = vm_hw_lua_cpu_extract_cpu(L, 1);
	bstring name = bfromcstr(lua_tostring(L, 2));

	if (biseqcstrcaseless(name, "A"))
		vm->registers[REG_A] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "B"))
		vm->registers[REG_B] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "C"))
		vm->registers[REG_C] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "X"))
		vm->registers[REG_X] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "Y"))
		vm->registers[REG_Y] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "Z"))
		vm->registers[REG_Z] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "I"))
		vm->registers[REG_I] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "J"))
		vm->registers[REG_J] = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "SP"))
		vm->sp = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "PC"))
		vm->pc = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "IA"))
		vm->ia = (uint16_t)lua_tonumber(L, 3);
	else if (biseqcstrcaseless(name, "EX"))
		vm->ex = (uint16_t)lua_tonumber(L, 3);
	else
		lua_pushnil(L);

	return 1;
}

int vm_hw_lua_cpu_handle_ram_get(lua_State* L)
{
	// Table is at 1, key is at 2.
	vm_t* vm = vm_hw_lua_cpu_extract_cpu(L, 1);
	if (!lua_isnumber(L, 2))
	{
		lua_pushstring(L, "ram access must be by numeric value (use array operator)");
		lua_error(L);
		return 0;
	}
	lua_pushnumber(L, vm->ram[(uint16_t)lua_tonumber(L, 2)]);
	return 1;
}

int vm_hw_lua_cpu_handle_ram_set(lua_State* L)
{
	// Table is at 1, key is at 2, value is at 3.
	vm_t* vm = vm_hw_lua_cpu_extract_cpu(L, 1);
	if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
	{
		lua_pushstring(L, "ram write must be by numeric value (use array operator / numeric value)");
		lua_error(L);
		return 0;
	}
	vm->ram[(uint16_t)lua_tonumber(L, 2)] = (uint16_t)lua_tonumber(L, 3);
	return 0;
}

int vm_hw_lua_cpu_disassemble(lua_State* L)
{
	vm_t* vm = vm_hw_lua_cpu_extract_cpu(L, 1);
	struct inst inst = vm_disassemble(vm, luaL_checknumber(L, 2), true);
	lua_newtable(L);
	lua_newtable(L);
	lua_pushnumber(L, inst.original.full);
	lua_setfield(L, -2, "full");
	lua_pushnumber(L, inst.original.op);
	lua_setfield(L, -2, "op");
	lua_pushnumber(L, inst.original.a);
	lua_setfield(L, -2, "a");
	lua_pushnumber(L, inst.original.b);
	lua_setfield(L, -2, "b");
	lua_setfield(L, -2, "original");
	lua_newtable(L);
	if (inst.pretty.op != NULL)
		lua_pushstring(L, inst.pretty.op->data);
	else
		lua_pushnil(L);
	lua_setfield(L, -2, "op");
	if (inst.pretty.a != NULL)
		lua_pushstring(L, inst.pretty.a->data);
	else
		lua_pushnil(L);
	lua_setfield(L, -2, "a");
	if (inst.pretty.b != NULL)
		lua_pushstring(L, inst.pretty.b->data);
	else
		lua_pushnil(L);
	lua_setfield(L, -2, "b");
	lua_setfield(L, -2, "pretty");
	lua_pushnumber(L, inst.op);
	lua_setfield(L, -2, "op");
	lua_pushnumber(L, inst.a);
	lua_setfield(L, -2, "a");
	lua_pushnumber(L, inst.b);
	lua_setfield(L, -2, "b");
	lua_pushnumber(L, inst.size);
	lua_setfield(L, -2, "size");
	lua_newtable(L);
	if (inst.size >= 1)
	{
		lua_pushnumber(L, inst.extra[0]);
		lua_rawseti(L, -2, 1);
	}
	if (inst.size >= 2)
	{
		lua_pushnumber(L, inst.extra[1]);
		lua_rawseti(L, -2, 2);
	}
	lua_setfield(L, -2, "extra");
	lua_newtable(L);
	if (inst.used[0])
		lua_pushnumber(L, inst.next[0]);
	else
		lua_pushnil(L);
	lua_rawseti(L, -2, 1);
	if (inst.used[1])
		lua_pushnumber(L, inst.next[1]);
	else
		lua_pushnil(L);
	lua_rawseti(L, -2, 2);
	lua_setfield(L, -2, "next");
	bdestroy(inst.pretty.op);
	bdestroy(inst.pretty.a);
	bdestroy(inst.pretty.b);
	return 1;
}

void vm_hw_lua_cpu_push_cpu(lua_State* L, vm_t* vm)
{
	int cpu, cpu_mt, registers, registers_mt, ram, ram_mt;

	// Create tables.
	lua_newtable(L);
	cpu = lua_gettop(L);
	lua_newtable(L);
	cpu_mt = lua_gettop(L);
	lua_newtable(L);
	registers = lua_gettop(L);
	lua_newtable(L);
	registers_mt = lua_gettop(L);
	lua_newtable(L);
	ram = lua_gettop(L);
	lua_newtable(L);
	ram_mt = lua_gettop(L);

	// Push userdata into metatables.
	lua_pushlightuserdata(L, vm);
	lua_rawseti(L, cpu_mt, 0);
	lua_pushlightuserdata(L, vm);
	lua_rawseti(L, ram_mt, 0);
	lua_pushlightuserdata(L, vm);
	lua_rawseti(L, registers_mt, 0);

	// Create the metatable functions.
	lua_pushcfunction(L, vm_hw_lua_cpu_handle_ram_get);
	lua_setfield(L, ram_mt, "__index");
	lua_pushcfunction(L, vm_hw_lua_cpu_handle_ram_set);
	lua_setfield(L, ram_mt, "__newindex");
	lua_pushcfunction(L, vm_hw_lua_cpu_handle_register_get);
	lua_setfield(L, registers_mt, "__index");
	lua_pushcfunction(L, vm_hw_lua_cpu_handle_register_set);
	lua_setfield(L, registers_mt, "__newindex");

	// Associate metatables.
	lua_pushvalue(L, cpu_mt);
	lua_setmetatable(L, cpu);
	lua_pushvalue(L, ram_mt);
	lua_setmetatable(L, ram);
	lua_pushvalue(L, registers_mt);
	lua_setmetatable(L, registers);

	// FIXME: Protect the metatables.
	//lua_pushboolean(hw->state, true);
	//lua_setfield(hw->state, ram_mt, "__metatable");
	//lua_pushboolean(hw->state, true);
	//lua_setfield(hw->state, registers_mt, "__metatable");

	// Put ram and registers into CPU.
	lua_pushvalue(L, ram);
	lua_setfield(L, cpu, "ram");
	lua_pushvalue(L, registers);
	lua_setfield(L, cpu, "registers");
	lua_pushcfunction(L, &vm_hw_lua_cpu_disassemble);
	lua_setfield(L, cpu, "disassemble");

	// Clean up stack.
	lua_pop(L, lua_gettop(L) - cpu);

	// CPU is now on top of the stack.
}