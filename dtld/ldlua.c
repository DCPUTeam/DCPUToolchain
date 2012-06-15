/**

	File:		ldlua.c

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides optimization infrastructure for Lua-based
			optimizers.

**/

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <dirent.portable.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <bstring.h>
#include <dcpu.h>
#include <debug.h>
#include <osutil.h>
#include <ppexprlua.h>
#include "ldlua.h"

bool modules_online;
list_t modules;

struct lua_optmst
{
	bstring name;
	lua_State* state;
};

size_t lua_optmst_meter(const void* el)
{
	return sizeof(struct lua_optmst);
}

struct lua_optmst* bin_lua_load(bstring name)
{
	bstring path, modtype;
	struct lua_optmst* om;
	int module;

	// Calculate full path to file.
	path = osutil_getmodulepath();
	bconchar(path, '/');
	bconcat(path, name);

	// Create the new lua optimization structure.
	om = malloc(sizeof(struct lua_optmst));
	om->name = name;
	om->state = lua_open();
	assert(om->state != NULL);
	luaL_openlibs(om->state);
	luaX_loadexpressionlib(om->state);

	// Execute the code in the new Lua context.
	if (luaL_dofile(om->state, path->data) != 0)
	{
		printd(LEVEL_ERROR, "lua error was %s.\n", lua_tostring(om->state, -1));

		// Return NULL.
		lua_close(om->state);
		free(om);
		bdestroy(path);
		return NULL;
	}

	// Load tables.
	lua_getglobal(om->state, "MODULE");
	module = lua_gettop(om->state);

	// Ensure module table was provided.
	if (lua_isnoneornil(om->state, module))
	{
		printd(LEVEL_ERROR, "failed to load optimizer module from %s.\n", path->data);

		// Return NULL.
		lua_close(om->state);
		free(om);
		bdestroy(path);
		return NULL;
	}

	// Check to see whether the module is
	// a preprocessor module.
	lua_getfield(om->state, module, "Type");
	modtype = bfromcstr(lua_tostring(om->state, -1));
	if (!biseqcstrcaseless(modtype, "Optimizer"))
	{
		// Return NULL.
		lua_pop(om->state, 1);
		lua_close(om->state);
		free(om);
		bdestroy(modtype);
		bdestroy(path);
		return NULL;
	}
	lua_pop(om->state, 1);
	bdestroy(modtype);

	// Return new debugger module.
	return om;
}

void bin_lua_init()
{
	DIR* dir;
	struct dirent* entry;
	struct lua_optmst* om;
	bstring name = NULL;
	bstring modpath = NULL;
	bstring ext = bfromcstr(".lua");

	// Initialize lists.
	list_init(&modules);
	list_attributes_copy(&modules, &lua_optmst_meter, 1);

	// Get the module path.
	modpath = osutil_getmodulepath();
	if (modpath == NULL)
	{
		bdestroy(ext);
		return;
	}

	// Attempt to open the module directory.
	dir = opendir(modpath->data);
	if (dir == NULL)
	{
		// The directory does not exist, so we don't load
		// any custom optimizer modules.
		modules_online = false;
		bdestroy(modpath);
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
#if defined(DT_REG)
		if (entry->d_type != DT_REG)
#elif defined(DT_DIR)
		if (entry->d_type == DT_DIR)
#elif defined(DT_UNKNOWN)
		if (entry->d_type == DT_UNKNOWN)
#else
#error Build system must support DT_REG, DT_DIR or DT_UNKNOWN in dirent.h.
#endif
		{
			// Not a file, skip over and then
			// continue.
			bdestroy(name);
			continue;
		}

		// Attempt to load the Lua file.
		printd(LEVEL_VERBOSE, "loading custom debugger module from: %s\n", name->data);
		om = bin_lua_load(name);
		if (om != NULL)
			list_append(&modules, om);

		// Free data.
		bdestroy(name);
	}

	// Free resources.
	closedir(dir);
}

/*
struct dbg_state* dbg_lua_extract_state(lua_State* L, int idx)
{
	struct dbg_state* state = NULL;
	lua_getmetatable(L, idx);
	lua_rawgeti(L, lua_gettop(L), 0);
	if (!lua_isuserdata(L, -1))
	{
		lua_pushstring(L, "state must be first parameter to function, or use : operator.");
		lua_error(L);
	}
	state = (struct dbg_state*)lua_touserdata(L, -1);
	lua_pop(L, 2);
	return state;
}
*/

void bin_lua_optimize(struct ldbin* bin)
{
	unsigned int i;
	struct lua_optmst* om;

	// Initialize if needed.
	if (!modules_online)
		bin_lua_init();

	// Loop through all of the modules.
	for (i = 0; i < list_size(&modules); i++)
	{
		om = list_get_at(&modules, i);

		// Set stack top (I don't know why the top of the
		// stack is negative!)
		lua_settop(om->state, 0);

		// Call the optimize function.
		lua_getglobal(om->state, "optimize");
		//dbg_lua_push_state(om, state, ud);
		//lua_pushnumber(om->state, pos);
		if (lua_pcall(om->state, 2, 0, 0) != 0)
		{
			printd(LEVEL_ERROR, "error: unable to call optimizer '%s'.\n", om->name);
			printd(LEVEL_ERROR, "%s\n", lua_tostring(om->state, -1));
			lua_pop(om->state, 2);
			continue;
		}

		printd(LEVEL_ERROR, "note: called optimizer '%s'.\n", om->name);
	}

	// Clean up.
}