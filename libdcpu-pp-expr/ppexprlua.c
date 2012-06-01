/**
 *
 *	File:		ppexprlua.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU-PP-Expr
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Defines the public API for exposing expression trees
 *			as Lua tables.
 *
 **/

#include <setjmp.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <simclist.h>
#include "dcpu.h"
#include "ppexpr.h"
#include "ppexprlua.h"
#include "parser.h"
#include "lexer.h"

#define EXPR_OP_ENTRIES 15

struct
{
	int op;
	const char* str;
} opmatch[EXPR_OP_ENTRIES] =
{
	{ EXPR_OP_INVALID, NULL },
	{ EXPR_OP_ADD, "+" },
	{ EXPR_OP_SUBTRACT, "-" },
	{ EXPR_OP_MULTIPLY, "" },
	{ EXPR_OP_DIVIDE, "" },
	{ EXPR_OP_MODULUS, "" },
	{ EXPR_OP_EQUALS, "" },
	{ EXPR_OP_NOT_EQUALS, "" },
	{ EXPR_OP_LESS_THAN, "" },
	{ EXPR_OP_LESS_EQUALS, "" },
	{ EXPR_OP_GREATER_THAN, "" },
	{ EXPR_OP_GREATER_EQUALS, "" },
	{ EXPR_OP_AND, "" },
	{ EXPR_OP_BOR, "" },
	{ EXPR_OP_XOR, "" }
};

struct expr* expr_lua_extract_expression(lua_State* L, int idx)
{
	struct expr* ex = NULL;
	lua_getmetatable(L, idx);
	lua_rawgeti(L, lua_gettop(L), 0);
	ex = (struct expr*)lua_touserdata(L, -1);
	lua_pop(L, 2);
	return ex;
}

struct evaluate_state
{
	struct expr* expr;
	jmp_buf jmp;
	lua_State* lua;
	int labelFunc;
};
list_t* evaluate_states = NULL;
size_t evaluate_state_meter(const void* el)
{
	return sizeof(struct evaluate_state);
}

void expr_lua_expression_self_error(int code, void* data)
{
	longjmp(((struct evaluate_state*)(list_get_at(evaluate_states, list_size(evaluate_states) - 1)))->jmp, 1);
}

uint16_t expr_lua_expression_self_label(bstring name)
{
	struct evaluate_state* state;
	uint16_t result;
	assert(list_size(evaluate_states) > 0);
	state = list_get_at(evaluate_states, list_size(evaluate_states) - 1);
	assert(state->labelFunc != -1);
	lua_pushvalue(state->lua, state->labelFunc);
	lua_pushstring(state->lua, name->data);
	if (lua_pcall(state->lua, 1, 1, 0) != 0)
	{
		lua_pushstring(state->lua, "calling label resolution function failed.");
		lua_error(state->lua);
		return 0;
	}
	result = (uint16_t)lua_tonumber(state->lua, -1);
	lua_pop(state->lua, 1);
	return result;
}

int expr_lua_expression_self_evaluate(lua_State* L)
{
	struct evaluate_state state;
	struct expr* ex;
	int result;
	uint16_t eval;

	// Check to make sure this function is being called correctly.
	if (!lua_istable(L, 1))
	{
		lua_pushstring(L, "evaluate() must have expression as first parameter (or be called with : operator)");
		lua_error(L);
	}

	// Initialize state list if needed.
	if (evaluate_states == NULL)
	{
		evaluate_states = malloc(sizeof(list_t));
		list_init(evaluate_states);
		list_attributes_copy(evaluate_states, &evaluate_state_meter, true);
	}

	// Extract expression.
	ex = expr_lua_extract_expression(L, 1);

	// Create a new evaluation state.
	state.expr = ex;
	state.lua = L;
	state.labelFunc = -1;
	result = setjmp(state.jmp);
	if (result != 0)
	{
		assert(list_size(evaluate_states) > 0);
		list_delete_at(evaluate_states, list_size(evaluate_states) - 1);
		lua_pushstring(L, "unable to evaluate expression.");
		lua_error(L);
	}

	// Evaluate the expression.
	if (lua_isfunction(L, 2))
	{
		lua_pushvalue(L, 2);
		state.labelFunc = lua_gettop(L);
		list_append(evaluate_states, &state);
		eval = expr_evaluate(ex, &expr_lua_expression_self_label, &expr_lua_expression_self_error);
		lua_pop(L, 1);
		lua_pushnumber(L, eval);
	}
	else
	{
		list_append(evaluate_states, &state);
		eval = expr_evaluate(ex, NULL, &expr_lua_expression_self_error);
		lua_pop(L, 1);
		lua_pushnumber(L, eval);
	}
	assert(list_size(evaluate_states) > 0);
	list_delete_at(evaluate_states, list_size(evaluate_states) - 1);
	return 1;
}

int expr_lua_expression_self_representation(lua_State* L)
{
	struct expr* ex;
	bstring result;
	if (!lua_istable(L, 1))
	{
		lua_pushstring(L, "representation() must be called with : syntax.");
		lua_error(L);
	}
	ex = expr_lua_extract_expression(L, 1);
	result = expr_representation(ex);
	lua_pushstring(L, result->data);
	bdestroy(result);
	return 1;
}

int expr_lua_handle_get(lua_State* L)
{
	// Table is at 1, key is at 2.
	struct expr* ex = expr_lua_extract_expression(L, 1);
	bstring name = bfromcstr(luaL_checkstring(L, 2));
	int i;
	bool found;

	if (biseqcstrcaseless(name, "a"))
		luaX_pushexpression(L, ex->a);
	else if (biseqcstrcaseless(name, "b"))
		luaX_pushexpression(L, ex->b);
	else if (biseqcstrcaseless(name, "type"))
	{
		switch (ex->type)
		{
			case EXPR_EXPR:
				lua_pushstring(L, "EXPRESSION");
				break;
			case EXPR_NUMBER:
				lua_pushstring(L, "NUMBER");
				break;
			case EXPR_LABEL:
				lua_pushstring(L, "LABEL");
				break;
			default:
				lua_pushstring(L, "unable to determine type of expression.");
				lua_error(L);
				return 0;
		}
	}
	else if (biseqcstrcaseless(name, "op"))
	{
		found = false;
		for (i = 0; i < EXPR_OP_ENTRIES; i++)
			if (ex->type == opmatch[i].op)
			{
				if (opmatch[i].str == NULL)
					lua_pushnil(L);
				else
					lua_pushstring(L, opmatch[i].str);
				found = true;
				break;
			}
		if (!found)
		{
			lua_pushstring(L, "unable to determine operation of expression.");
			lua_error(L);
			return 0;
		}
	}
	else if (biseqcstrcaseless(name, "evaluate"))
		lua_pushcfunction(L, &expr_lua_expression_self_evaluate);
	else if (biseqcstrcaseless(name, "representation"))
		lua_pushcfunction(L, &expr_lua_expression_self_representation);
	else
		lua_pushnil(L);

	return 1;
}

int expr_lua_handle_set(lua_State* L)
{
	// Table is at 1, key is at 2, value is at 3.
	struct expr* ex = expr_lua_extract_expression(L, 1);
	bstring name = bfromcstr(luaL_checkstring(L, 2));
	int i;
	bool found;

	if (biseqcstrcaseless(name, "a"))
	{
		if (lua_isuserdata(L, 3))
			ex->a = expr_lua_extract_expression(L, 3);
		else
		{
			lua_pushstring(L, "value must be an expression in order to set 'a'");
			lua_error(L);
			return 0;
		}
	}
	else if (biseqcstrcaseless(name, "b"))
	{
		if (lua_isuserdata(L, 3))
			ex->b = expr_lua_extract_expression(L, 3);
		else
		{
			lua_pushstring(L, "value must be an expression in order to set 'b'");
			lua_error(L);
			return 0;
		}
	}
	else if (biseqcstrcaseless(name, "op"))
	{
		for (i = 0; i < EXPR_OP_ENTRIES; i++)
			if (strcmp(lua_tostring(L, 3), opmatch[i].str) == 0)
			{
				ex->op = opmatch[i].op;
				found = true;
				break;
			}
		if (!found)
		{
			lua_pushstring(L, "unable to determine operation value to set.");
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_pushstring(L, "unable to set requested property in expression object.");
		lua_error(L);
		return 0;
	}

	return 0;
}

int ppexpr_yyparse(struct expr** expr, void* scanner);

int expr_lua_expression_create(lua_State* L)
{
	char* str = strdup(luaL_checkstring(L, 1));
	struct expr* ex = NULL;
	yyscan_t scanner;
	YY_BUFFER_STATE state;

	// Initialize lexer.
	if (ppexpr_yylex_init(&scanner))
	{
		free(str);
		lua_pushstring(L, "unable to lex expression string (initialization failed).");
		lua_error(L);
		return 0;
	}

	// Lex string expression.
	state = ppexpr_yy_scan_string(str, scanner);
	free(str);

	// Parse string expression.
	if (ppexpr_yyparse(&ex, scanner))
	{
		lua_pushstring(L, "unable to parse expression string (parsing failed).");
		lua_error(L);
		return 0;
	}

	// Clean up.
	ppexpr_yy_delete_buffer(state, scanner);
	ppexpr_yylex_destroy(scanner);

	// Return the result.
	if (ex == NULL)
	{
		lua_pushstring(L, "unable to parse expression string (result is NULL).");
		lua_error(L);
		return 0;
	}
	else
		luaX_pushexpression(L, ex);
	return 1;
}

int luaX_loadexpressionlib(lua_State* L)
{
	// Set expression_create global variable.
	lua_pushcfunction(L, &expr_lua_expression_create);
	lua_setglobal(L, "expression_create");

	return 0;
}

int luaX_pushexpression(lua_State* L, struct expr* expr)
{
	int ex, ex_mt;

	// Create tables.
	lua_newtable(L);
	ex = lua_gettop(L);
	lua_newtable(L);
	ex_mt = lua_gettop(L);

	// Push userdata into metatable.
	lua_pushlightuserdata(L, expr);
	lua_rawseti(L, ex_mt, 0);

	// Push index / newindex metamethods.
	lua_pushcfunction(L, &expr_lua_handle_get);
	lua_setfield(L, ex_mt, "__index");
	lua_pushcfunction(L, &expr_lua_handle_set);
	lua_setfield(L, ex_mt, "__newindex");

	// Associate metatables.
	lua_pushvalue(L, ex_mt);
	lua_setmetatable(L, ex);

	// Clean up stack.
	lua_pop(L, lua_gettop(L) - ex);

	return 1;
}