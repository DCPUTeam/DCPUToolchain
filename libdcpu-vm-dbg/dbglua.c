/**

    File:       dbglua.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-VM-Dbg

    Authors:    James Rhodes

    Description:    Defines the public API for the debugger to load
            custom Lua modules.

**/

#include <assert.h>
#include <dirent.portable.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <bstring.h>
#include <simclist.h>
#include <osutil.h>
#include <ppexprlua.h>
#include <debug.h>
#include <stdlib.h>
#include <hwluacpu.h>
#include <ddata.h>
#include <luaglob.h>
#include "dbglua.h"
#include "dcpu.h"
#include "dcpuexec.h"

#define HANDLER_TABLE_COMMANDS_NAME "__cmdhandlers"
#define HANDLER_TABLE_HOOKS_NAME "__hookhandlers"
#define HANDLER_TABLE_SYMBOLS_NAME "__symhandlers"
#define HANDLER_FIELD_FUNCTION_NAME "function"
#define HANDLER_ENTRY_SYMBOL_HOOK "symhook"

bool modules_online;
list_t modules;

struct lua_debugst
{
    lua_State* state;
};

size_t lua_debugst_meter(const void* el)
{
    return sizeof(struct lua_debugst);
}

int dbg_lua_add_base(lua_State* L, const char* table)
{
    int handlers, entry;
    bstring lowered;
    lowered = bfromcstr(luaL_checkstring(L, 1));
    btolower(lowered);
    lua_getglobal(L, table);
    handlers = lua_gettop(L);
    lua_newtable(L);
    entry = lua_gettop(L);
    lua_pushvalue(L, 2);
    lua_setfield(L, entry, HANDLER_FIELD_FUNCTION_NAME);
    // entry is now at the top of the stack.
    lua_setfield(L, handlers, lowered->data);
    printd(LEVEL_DEBUG, "registered debugger command / hook '%s' with custom Lua handler.\n", lowered->data);
    lua_pop(L, 1);
    bdestroy(lowered);
    return 0;
}

int dbg_lua_add_command(lua_State* L)
{
    return dbg_lua_add_base(L, HANDLER_TABLE_COMMANDS_NAME);
}

int dbg_lua_add_hook(lua_State* L)
{
    return dbg_lua_add_base(L, HANDLER_TABLE_HOOKS_NAME);
}

int dbg_lua_add_symbol_hook(lua_State* L)
{
    int handlers, entry;
    lua_getglobal(L, HANDLER_TABLE_SYMBOLS_NAME);
    handlers = lua_gettop(L);
    lua_newtable(L);
    entry = lua_gettop(L);
    lua_pushvalue(L, 1);
    lua_setfield(L, entry, HANDLER_FIELD_FUNCTION_NAME);
    // entry is now at the top of the stack.
    lua_setfield(L, handlers, HANDLER_ENTRY_SYMBOL_HOOK);
    printd(LEVEL_DEBUG, "registered debugger symbol hook with custom Lua handler.\n");
    lua_pop(L, 1);
    return 0;
}

struct lua_debugst* dbg_lua_load(bstring name)
{
    bstring path, modtype;
    struct lua_debugst* ds;
    int module;

    // Calculate full path to file.
    path = osutil_getmodulepath();
    bconchar(path, '/');
    bconcat(path, name);

    // Create the new lua preprocessor structure.
    ds = malloc(sizeof(struct lua_debugst));
    ds->state = lua_open();
    assert(ds->state != NULL);
    luaL_openlibs(ds->state);
    luaX_loadexpressionlib(ds->state);

    // Load globals.
    dcpu_lua_set_constants(ds->state);

    // Execute the code in the new Lua context.
    if (luaL_dofile(ds->state, path->data) != 0)
    {
        printd(LEVEL_ERROR, "lua error was %s.\n", lua_tostring(ds->state, -1));

        // Return NULL.
        lua_close(ds->state);
        free(ds);
        bdestroy(path);
        return NULL;
    }

    // Load tables.
    lua_getglobal(ds->state, "MODULE");
    module = lua_gettop(ds->state);

    // Ensure module table was provided.
    if (lua_isnoneornil(ds->state, module))
    {
        printd(LEVEL_ERROR, "failed to load debugger module from %s.\n", path->data);

        // Return NULL.
        lua_close(ds->state);
        free(ds);
        bdestroy(path);
        return NULL;
    }

    // Check to see whether the module is
    // a preprocessor module.
    lua_getfield(ds->state, module, "Type");
    modtype = bfromcstr(lua_tostring(ds->state, -1));
    if (!biseqcstrcaseless(modtype, "Debugger"))
    {
        // Return NULL.
        lua_pop(ds->state, 1);
        lua_close(ds->state);
        free(ds);
        bdestroy(modtype);
        bdestroy(path);
        return NULL;
    }
    lua_pop(ds->state, 1);
    bdestroy(modtype);

    // Create the handler tables.
    lua_newtable(ds->state);
    lua_setglobal(ds->state, HANDLER_TABLE_COMMANDS_NAME);
    lua_newtable(ds->state);
    lua_setglobal(ds->state, HANDLER_TABLE_HOOKS_NAME);
    lua_newtable(ds->state);
    lua_setglobal(ds->state, HANDLER_TABLE_SYMBOLS_NAME);

    // Set the global functions.
    lua_pushcfunction(ds->state, &dbg_lua_add_command);
    lua_setglobal(ds->state, "add_command");
    lua_pushcfunction(ds->state, &dbg_lua_add_hook);
    lua_setglobal(ds->state, "add_hook");
    lua_pushcfunction(ds->state, &dbg_lua_add_symbol_hook);
    lua_setglobal(ds->state, "add_symbol_hook");

    // Run the setup function.
    lua_getglobal(ds->state, "setup");
    if (lua_pcall(ds->state, 0, 0, 0) != 0)
    {
        printd(LEVEL_ERROR, "failed to run setup() in debugger module from %s.\n", path->data);
        printd(LEVEL_ERROR, "lua error was %s.\n", lua_tostring(ds->state, -1));
    }

    // Unset the global functions.
    lua_pushnil(ds->state);
    lua_setglobal(ds->state, "add_command");
    lua_pushnil(ds->state);
    lua_setglobal(ds->state, "add_hook");
    lua_pushnil(ds->state);
    lua_setglobal(ds->state, "add_symbol_hook");

    // Pop tables from stack.
    lua_pop(ds->state, 2);

    // Return new debugger module.
    return ds;
}

void dbg_lua_init()
{
    DIR* dir;
    struct dirent* entry;
    struct lua_debugst* dsm;
    bstring name = NULL;
    bstring modpath = NULL;
    bstring ext = bfromcstr(".lua");

    // Initialize lists.
    list_init(&modules);
    list_attributes_copy(&modules, &lua_debugst_meter, 1);

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
        // any custom debugger modules.
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
        dsm = dbg_lua_load(name);
        if (dsm != NULL)
            list_append(&modules, dsm);

        // Free data.
        bdestroy(name);
    }

    // Free resources.
    closedir(dir);
}

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

void* dbg_lua_extract_state_ud(lua_State* L, int idx)
{
    void* ud = NULL;
    lua_getmetatable(L, idx);
    lua_rawgeti(L, lua_gettop(L), 1);
    if (!lua_isuserdata(L, -1))
    {
        lua_pushstring(L, "state must be first parameter to function, or use : operator.");
        lua_error(L);
    }
    ud = lua_touserdata(L, -1);
    lua_pop(L, 2);
    return ud;
}

extern int ddbg_return_code;

int dbg_lua_handle_break(lua_State* L)
{
    struct dbg_state* state = dbg_lua_extract_state(L, 1);
    void* ud = dbg_lua_extract_state_ud(L, 1);
    state->dbg_lua_break();
    if (lua_isboolean(L, 2))
    {
        if (lua_toboolean(L, 2))
            ddbg_return_code = 0;
        else
            ddbg_return_code = 1;
    }
    else if (lua_isnumber(L, 2))
        ddbg_return_code = (int)lua_tonumber(L, 2);
    return 0;
}

int dbg_lua_handle_run(lua_State* L)
{
    struct dbg_state* state = dbg_lua_extract_state(L, 1);
    void* ud = dbg_lua_extract_state_ud(L, 1);
    state->dbg_lua_run();
    return 0;
}

int dbg_lua_handle_symbols(lua_State* L)
{
    struct dbg_state* state = dbg_lua_extract_state(L, 1);
    void* ud = dbg_lua_extract_state_ud(L, 1);
    list_t* symbols = state->dbg_lua_get_symbols();
    struct dbg_sym* current = NULL;
    struct dbg_sym_payload_string* payload = NULL;
    int tbl;
    lua_newtable(L);
    if (symbols == NULL)
        return 1;
    tbl = lua_gettop(L);
    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
    {
        current = (struct dbg_sym*)list_iterator_next(symbols);
        if (current->type != DBGFMT_SYMBOL_STRING)
            continue;
        payload = (struct dbg_sym_payload_string*)current->payload;
        if (payload->data == NULL)
            continue;
        lua_newtable(L);
        lua_pushstring(L, payload->data->data);
        lua_setfield(L, -2, "data");
        lua_pushnumber(L, payload->address);
        lua_setfield(L, -2, "address");
        lua_rawseti(L, tbl, lua_objlen(L, tbl) + 1);
    }
    list_iterator_stop(symbols);
    return 1;
}

int dbg_lua_handle_lines(lua_State* L)
{
    struct dbg_state* state = dbg_lua_extract_state(L, 1);
    void* ud = dbg_lua_extract_state_ud(L, 1);
    list_t* symbols = state->dbg_lua_get_symbols();
    struct dbg_sym* current = NULL;
    struct dbg_sym_payload_line* payload = NULL;
    int tbl;
    lua_newtable(L);
    if (symbols == NULL)
        return 1;
    tbl = lua_gettop(L);
    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
    {
        current = (struct dbg_sym*)list_iterator_next(symbols);
        if (current->type != DBGFMT_SYMBOL_LINE)
            continue;
        payload = (struct dbg_sym_payload_line*)current->payload;
        if (payload->path == NULL)
            continue;
        lua_newtable(L);
        lua_pushstring(L, payload->path->data);
        lua_setfield(L, -2, "file");
        lua_pushnumber(L, payload->lineno);
        lua_setfield(L, -2, "line");
        lua_pushnumber(L, payload->address);
        lua_setfield(L, -2, "address");
        lua_rawseti(L, tbl, lua_objlen(L, tbl) + 1);
    }
    list_iterator_stop(symbols);
    return 1;
}

void dbg_lua_handle_hook(struct dbg_state* state, void* ud, freed_bstring type, uint16_t pos);

int dbg_lua_handle_raise(lua_State* L)
{
    struct dbg_state* state = dbg_lua_extract_state(L, 1);
    void* ud = dbg_lua_extract_state_ud(L, 1);
    dbg_lua_handle_hook(state, NULL, bautofree(bfromcstr(luaL_checkstring(L, 2))), luaL_optinteger(L, 3, 0));
    return 0;
}

void dbg_lua_push_state(struct lua_debugst* ds, struct dbg_state* state, void* ud)
{
    int tbl, tbl_mt;

    // Create the new table and metatable.
    lua_newtable(ds->state);
    tbl = lua_gettop(ds->state);
    lua_newtable(ds->state);
    tbl_mt = lua_gettop(ds->state);

    // Push userdata into metatable.
    lua_pushlightuserdata(ds->state, state);
    lua_rawseti(ds->state, tbl_mt, 0);
    lua_pushlightuserdata(ds->state, ud);
    lua_rawseti(ds->state, tbl_mt, 1);

    // Push C functions into table.
    lua_pushcfunction(ds->state, &dbg_lua_handle_break);
    lua_setfield(ds->state, tbl, "_break");
    lua_pushcfunction(ds->state, &dbg_lua_handle_run);
    lua_setfield(ds->state, tbl, "run");
    lua_pushcfunction(ds->state, &dbg_lua_handle_symbols);
    lua_setfield(ds->state, tbl, "symbols");
    lua_pushcfunction(ds->state, &dbg_lua_handle_lines);
    lua_setfield(ds->state, tbl, "lines");
    lua_pushcfunction(ds->state, &dbg_lua_handle_raise);
    lua_setfield(ds->state, tbl, "raise");

    // Push CPU state into table.
    vm_hw_lua_cpu_push_cpu(ds->state, state->get_vm());
    lua_setfield(ds->state, tbl, "cpu");

    // Associate metatables.
    lua_pushvalue(ds->state, tbl_mt);
    lua_setmetatable(ds->state, tbl);

    // Clean up stack.
    lua_pop(ds->state, lua_gettop(ds->state) - tbl);

    // New table is now at the top of the stack.
}

void dbg_lua_handle_command(struct dbg_state* state, void* ud, freed_bstring name, list_t* parameters)
{
    struct lua_debugst* ds;
    struct customarg_entry* carg;
    char* cstr;
    unsigned int i, k;
    int paramtbl;

    // Convert the name to lowercase.
    btolower(name.ref);
    cstr = bstr2cstr(name.ref, '0');

    // Loop through all of the modules.
    for (k = 0; k < list_size(&modules); k++)
    {
        ds = list_get_at(&modules, k);

        // Set stack top (I don't know why the top of the
        // stack is negative!)
        lua_settop(ds->state, 0);

        // Search handler table for entries.
        lua_getglobal(ds->state, HANDLER_TABLE_COMMANDS_NAME);
        lua_getfield(ds->state, -1, cstr);
        if (!lua_istable(ds->state, -1))
        {
            // No such entry.
            lua_pop(ds->state, 2);
            continue;
        }

        // Call the handler function.
        lua_getfield(ds->state, -1, HANDLER_FIELD_FUNCTION_NAME);
        dbg_lua_push_state(ds, state, ud);
        lua_newtable(ds->state);
        paramtbl = lua_gettop(ds->state);
        for (i = 0; i < list_size(parameters); i++)
        {
            carg = list_get_at(parameters, i);

            lua_newtable(ds->state);
            if (carg->type == DBG_CUSTOMARG_TYPE_PATH)
                lua_pushstring(ds->state, "PATH");
            else if (carg->type == DBG_CUSTOMARG_TYPE_PARAM)
                lua_pushstring(ds->state, "PARAM");
            else if (carg->type == DBG_CUSTOMARG_TYPE_STRING)
                lua_pushstring(ds->state, "STRING");
            else
                lua_pushstring(ds->state, "NUMBER");
            lua_setfield(ds->state, -2, "type");
            if (carg->type == DBG_CUSTOMARG_TYPE_PATH)
                lua_pushstring(ds->state, carg->path->data);
            else if (carg->type == DBG_CUSTOMARG_TYPE_PARAM)
                lua_pushstring(ds->state, carg->param->data);
            else if (carg->type == DBG_CUSTOMARG_TYPE_STRING)
                lua_pushstring(ds->state, carg->string->data);
            else
                lua_pushnumber(ds->state, carg->number);
            lua_setfield(ds->state, -2, "value");
            lua_rawseti(ds->state, paramtbl, i + 1);
        }
        if (lua_pcall(ds->state, 2, 0, 0) != 0)
        {
            printd(LEVEL_ERROR, "error: unable to call debugger command handler for '%s'.\n", name.ref->data);
            printd(LEVEL_ERROR, "%s\n", lua_tostring(ds->state, -1));
            bautodestroy(name);
            bcstrfree(cstr);
            lua_pop(ds->state, 2);
            list_iterator_stop(&modules);
            list_destroy(parameters);
            return;
        }

        bautodestroy(name);
        bcstrfree(cstr);
        lua_pop(ds->state, 2);
        list_iterator_stop(&modules);
        list_destroy(parameters);

        // The command may have started the virtual machine, check the
        // status of the VM and execute if needed.
        if (state->get_vm()->halted == false)
            vm_execute(state->get_vm(), NULL);

        return;
    }

    // There is no command to handle this.
    printd(LEVEL_ERROR, "no such command found.\n");

    // Clean up.
    list_destroy(parameters);
    bautodestroy(name);
    bcstrfree(cstr);
}

void dbg_lua_handle_hook(struct dbg_state* state, void* ud, freed_bstring type, uint16_t pos)
{
    struct lua_debugst* ds;
    unsigned int i;
    char* cstr;

    // Convert the name to lowercase.
    btolower(type.ref);
    cstr = bstr2cstr(type.ref, '0');

    printd(LEVEL_EVERYTHING, "firing hook %s.\n", cstr);

    // Loop through all of the modules.
    for (i = 0; i < list_size(&modules); i++)
    {
        ds = list_get_at(&modules, i);

        // Set stack top (I don't know why the top of the
        // stack is negative!)
        lua_settop(ds->state, 0);

        // Search handler table for entries.
        lua_getglobal(ds->state, HANDLER_TABLE_HOOKS_NAME);
        lua_getfield(ds->state, -1, cstr);
        if (!lua_istable(ds->state, -1))
        {
            // No such entry.
            printd(LEVEL_EVERYTHING, "no matching hook for %s.\n", cstr);
            lua_pop(ds->state, 2);
            continue;
        }

        // Call the handler function.
        printd(LEVEL_EVERYTHING, "calling hook %s.\n", cstr);
        lua_getfield(ds->state, -1, HANDLER_FIELD_FUNCTION_NAME);
        dbg_lua_push_state(ds, state, ud);
        lua_pushnumber(ds->state, pos);
        if (lua_pcall(ds->state, 2, 0, 0) != 0)
        {
            printd(LEVEL_ERROR, "error: unable to call debugger hook handler for '%s'.\n", type.ref->data);
            printd(LEVEL_ERROR, "%s\n", lua_tostring(ds->state, -1));
            lua_pop(ds->state, 2);
            continue;
        }
    }

    // Clean up.
    bcstrfree(cstr);
    bautodestroy(type);
}

void dbg_lua_handle_hook_symbol(struct dbg_state* state, void* ud, freed_bstring symbol)
{
    struct lua_debugst* ds;
    unsigned int i;

    // Loop through all of the modules.
    for (i = 0; i < list_size(&modules); i++)
    {
        ds = list_get_at(&modules, i);

        // Set stack top (I don't know why the top of the
        // stack is negative!)
        lua_settop(ds->state, 0);

        // Search handler table for entries.
        lua_getglobal(ds->state, HANDLER_TABLE_SYMBOLS_NAME);
        lua_getfield(ds->state, -1, HANDLER_ENTRY_SYMBOL_HOOK);
        if (!lua_istable(ds->state, -1))
        {
            // No such entry.
            lua_pop(ds->state, 2);
            continue;
        }

        // Call the handler function.
        lua_getfield(ds->state, -1, HANDLER_FIELD_FUNCTION_NAME);
        dbg_lua_push_state(ds, state, ud);
        lua_pushstring(ds->state, symbol.ref->data);
        if (lua_pcall(ds->state, 2, 0, 0) != 0)
        {
            printd(LEVEL_ERROR, "error: unable to call debugger symbol hook handler.\n");
            printd(LEVEL_ERROR, "%s\n", lua_tostring(ds->state, -1));
            lua_pop(ds->state, 2);
            continue;
        }
    }

    // Clean up.
    bautodestroy(symbol);
}
