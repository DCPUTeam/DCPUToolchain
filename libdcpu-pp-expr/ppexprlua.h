/**
 *
 *	File:		ppexprlua.h
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU-PP-Expr
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Declares the public API for exposing expression trees
 *			as Lua tables.
 *
 **/

#ifndef __DCPU_LIBDCPU_PP_EXPR_LUA_H
#define __DCPU_LIBDCPU_PP_EXPR_LUA_H

#include <lua.h>
#include "ppexpr.h"

int luaX_loadexpressionlib(lua_State* L);
int luaX_pushexpression(lua_State* L, struct expr* expr);

#endif