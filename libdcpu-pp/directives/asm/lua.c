///
/// @addtogroup LibDCPU-PP
/// @{
///
/// @file
/// @brief Preprocessor handler for Lua modules in assembly.
/// @author James Rhodes
/// 
/// This file was adapted from the previous implementation and thus is a horrible
/// bastardization of code.  Modification may lead to serious injury.
///

///
/// @if HORRIBLE_CODE
///

#include "../asm.h"
#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <derr.h>
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
#include <luaglob.h>
#include <dcpu.h>

struct pp_hooks
{
    state_t* state;
    void (*pp_lua_print)(const char* text, void* ud);
    void (*pp_lua_print_line)(const char* text, void* ud);
    void (*pp_lua_scope_enter)(bool output, void* ud);
    void (*pp_lua_scope_exit)(void* ud);
    void (*pp_lua_add_symbol)(const char* symbol, void* ud);
};

void pp_lua_handle(struct pp_hooks* hooks, void* scanner, bstring name, list_t* parameters);

#define HANDLER_TABLE_NAME "__handlers"
#define HANDLER_FIELD_FUNCTION_NAME "function"

bool modules_online;
list_t modules;
struct pp_hooks lstate;

struct lua_preproc
{
    lua_State* state;
};

size_t lua_preproc_meter(const void* el)
{
    return sizeof(struct lua_preproc);
}

static void lua_handle_wrapper(state_t* state, match_t* match, bool* reprocess)
{
    list_t* parameters = ppparam_get(state);
    pp_lua_handle(&lstate, NULL, bstrcpy(match->userdata), parameters);
    // FIXME?
    //list_destroy(parameters);
    //free(parameters);
}

static void lua_handle_wrapper_term(state_t* state, match_t* match, bool* reprocess)
{
    list_t* parameters = malloc(sizeof(list_t));
    list_init(parameters);
    pp_lua_handle(&lstate, NULL, bstrcpy(match->userdata), parameters);
    // FIXME?
    //list_destroy(parameters);
    //free(parameters);
}

int pp_lua_add_preprocessor_directive(lua_State* L)
{
    int handlers, entry;
    bstring lowered;
    bstring concat;
    match_t* match;
    lowered = bfromcstr(luaL_checkstring(L, 1));
    btolower(lowered);
    lua_getglobal(L, HANDLER_TABLE_NAME);
    handlers = lua_gettop(L);
    lua_newtable(L);
    entry = lua_gettop(L);
    lua_pushvalue(L, 2);
    lua_setfield(L, entry, HANDLER_FIELD_FUNCTION_NAME);
    // entry is now at the top of the stack.
    lua_setfield(L, handlers, lowered->data);
    printd(LEVEL_DEBUG, "registered preprocessor directive '%s' with custom Lua handler.\n", lowered->data);
    lua_pop(L, 1);

    //
    // HACK: Add the handle wrapper to the list of preprocessor directives.
    //
    concat = bfromcstr(".");
    bconcat(concat, lowered);
    bconchar(concat, ' ');
    match = malloc(sizeof(match_t));
    match->text = bautofree(concat);
    match->handler = lua_handle_wrapper;
    match->userdata = lowered;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(lstate.state, match);
    concat = bfromcstr(".");
    bconcat(concat, lowered);
    bconchar(concat, '\n');
    match = malloc(sizeof(match_t));
    match->text = bautofree(concat);
    match->handler = lua_handle_wrapper_term;
    match->userdata = lowered;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(lstate.state, match);
    concat = bfromcstr(".");
    bconcat(concat, lowered);
    bconchar(concat, '\r');
    match = malloc(sizeof(match_t));
    match->text = bautofree(concat);
    match->handler = lua_handle_wrapper_term;
    match->userdata = lowered;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(lstate.state, match);
    
    return 0;
}

struct lua_preproc* pp_lua_load(bstring name)
{
    bstring path, modtype;
    struct lua_preproc* pp;
    int module;

    // Calculate full path to file.
    path = osutil_getmodulepath();
    bconchar(path, '/');
    bconcat(path, name);

    // Create the new lua preprocessor structure.
    pp = malloc(sizeof(struct lua_preproc));
    pp->state = luaL_newstate();
    assert(pp->state != NULL);
    luaL_openlibs(pp->state);
    luaX_loadexpressionlib(pp->state);

    // Load globals.
    dcpu_lua_set_constants(pp->state);

    // Execute the code in the new Lua context.
    if (luaL_dofile(pp->state, path->data) != 0)
    {
        printd(LEVEL_ERROR, "lua error was %s.\n", lua_tostring(pp->state, -1));

        // Return NULL.
        lua_close(pp->state);
        free(pp);
        bdestroy(path);
        return NULL;
    }

    // Load tables.
    lua_getglobal(pp->state, "MODULE");
    module = lua_gettop(pp->state);

    // Ensure module table was provided.
    if (lua_isnoneornil(pp->state, module))
    {
        printd(LEVEL_ERROR, "failed to load preprocessor module from %s.\n", path->data);

        // Return NULL.
        lua_close(pp->state);
        free(pp);
        bdestroy(path);
        return NULL;
    }

    // Check to see whether the module is
    // a preprocessor module.
    lua_getfield(pp->state, module, "Type");
    modtype = bfromcstr(lua_tostring(pp->state, -1));
    if (!biseqcstrcaseless(modtype, "Preprocessor"))
    {
        // Return NULL.
        lua_pop(pp->state, 1);
        lua_close(pp->state);
        free(pp);
        bdestroy(modtype);
        bdestroy(path);
        return NULL;
    }
    lua_pop(pp->state, 1);
    bdestroy(modtype);

    // Create the handler table.
    lua_newtable(pp->state);
    lua_setglobal(pp->state, HANDLER_TABLE_NAME);

    // Set the global add_preprocessor_directive function.
    lua_pushcfunction(pp->state, &pp_lua_add_preprocessor_directive);
    lua_setglobal(pp->state, "add_preprocessor_directive");

    // Run the setup function.
    lua_getglobal(pp->state, "setup");
    if (lua_pcall(pp->state, 0, 0, 0) != 0)
    {
        printd(LEVEL_ERROR, "failed to run setup() in preprocessor module from %s.\n", path->data);
    }

    // Unset the add_preprocessor_directive function.
    lua_pushnil(pp->state);
    lua_setglobal(pp->state, "add_preprocessor_directive");

    // Pop tables from stack.
    lua_pop(pp->state, 2);

    // Return new preprocessor module.
    return pp;
}

void pp_lua_init()
{
    DIR* dir;
    struct dirent* entry;
    struct lua_preproc* ppm;
    bstring name = NULL;
    bstring modpath = NULL;
    bstring ext = bfromcstr(".lua");

    // Initialize lists.
    list_init(&modules);
    list_attributes_copy(&modules, &lua_preproc_meter, 1);

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
        // any custom preprocessor modules.
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
        printd(LEVEL_DEBUG, "loading custom preprocessor module from: %s\n", name->data);
        ppm = pp_lua_load(name);
        if (ppm != NULL)
            list_append(&modules, ppm);

        // Free data.
        bdestroy(name);
    }

    // Free resources.
    closedir(dir);
}

///
/// @endif
///

void ppwrap_print(const char* text, void* ud)
{
    ppimpl_printf(lstate.state, "%s", text);
}

void ppwrap_print_line(const char* text, void* ud)
{
    ppimpl_printf(lstate.state, "%s\n", text);
}

void ppwrap_scope_enter(bool output, void* ud)
{
    ppimpl_push_scope(lstate.state, output);
}

void ppwrap_scope_exit(void* ud)
{
    ppimpl_pop_scope(lstate.state);
}

void ppwrap_add_symbol(const char* symbol, void* ud)
{
    ppimpl_printf(lstate.state, ".SYMBOL %s\n", symbol);
}

void ppimpl_asm_lua_register(state_t* state)
{
    lstate.state = state;
    lstate.pp_lua_print = &ppwrap_print;
    lstate.pp_lua_print_line = &ppwrap_print_line;
    lstate.pp_lua_scope_enter = &ppwrap_scope_enter;
    lstate.pp_lua_scope_exit = &ppwrap_scope_exit;
    lstate.pp_lua_add_symbol = &ppwrap_add_symbol;
    pp_lua_init();
}

///
/// @if HORRIBLE_CODE
///

struct pp_hooks* pp_lua_extract_state(lua_State* L, int idx)
{
    struct pp_hooks* state = NULL;
    lua_getmetatable(L, idx);
    lua_rawgeti(L, lua_gettop(L), 0);
    if (!lua_isuserdata(L, -1))
    {
        lua_pushstring(L, "state must be first parameter to function, or use : operator.");
        lua_error(L);
    }
    state = (struct pp_hooks*)lua_touserdata(L, -1);
    lua_pop(L, 2);
    return state;
}

void* pp_lua_extract_state_ud(lua_State* L, int idx)
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

int pp_lua_handle_print(lua_State* L)
{
    struct pp_hooks* state = pp_lua_extract_state(L, 1);
    void* ud = pp_lua_extract_state_ud(L, 1);
    state->pp_lua_print(luaL_checkstring(L, 2), ud);
    return 0;
}

int pp_lua_handle_print_line(lua_State* L)
{
    struct pp_hooks* state = pp_lua_extract_state(L, 1);
    void* ud = pp_lua_extract_state_ud(L, 1);
    state->pp_lua_print_line(luaL_checkstring(L, 2), ud);
    return 0;
}

int pp_lua_handle_scope_enter(lua_State* L)
{
    struct pp_hooks* state = pp_lua_extract_state(L, 1);
    void* ud = pp_lua_extract_state_ud(L, 1);
    state->pp_lua_scope_enter(lua_toboolean(L, 2), ud);
    return 0;
}

int pp_lua_handle_scope_exit(lua_State* L)
{
    struct pp_hooks* state = pp_lua_extract_state(L, 1);
    void* ud = pp_lua_extract_state_ud(L, 1);
    state->pp_lua_scope_exit(ud);
    return 0;
}

int pp_lua_handle_add_symbol(lua_State* L)
{
    struct pp_hooks* state = pp_lua_extract_state(L, 1);
    void* ud = pp_lua_extract_state_ud(L, 1);
    state->pp_lua_add_symbol(luaL_checkstring(L, 2), ud);
    return 0;
}

void pp_lua_push_state(struct lua_preproc* pp, struct pp_hooks* state, void* ud)
{
    int tbl, tbl_mt;

    // Create the new table and metatable.
    lua_newtable(pp->state);
    tbl = lua_gettop(pp->state);
    lua_newtable(pp->state);
    tbl_mt = lua_gettop(pp->state);

    // Push userdata into metatable.
    lua_pushlightuserdata(pp->state, state);
    lua_rawseti(pp->state, tbl_mt, 0);
    lua_pushlightuserdata(pp->state, ud);
    lua_rawseti(pp->state, tbl_mt, 1);

    // Push C functions into table.
    lua_pushcfunction(pp->state, &pp_lua_handle_print);
    lua_setfield(pp->state, tbl, "print");
    lua_pushcfunction(pp->state, &pp_lua_handle_print_line);
    lua_setfield(pp->state, tbl, "print_line");
    lua_pushcfunction(pp->state, &pp_lua_handle_scope_enter);
    lua_setfield(pp->state, tbl, "scope_enter");
    lua_pushcfunction(pp->state, &pp_lua_handle_scope_exit);
    lua_setfield(pp->state, tbl, "scope_exit");
    lua_pushcfunction(pp->state, &pp_lua_handle_add_symbol);
    lua_setfield(pp->state, tbl, "add_symbol");

    // Associate metatables.
    lua_pushvalue(pp->state, tbl_mt);
    lua_setmetatable(pp->state, tbl);

    // Clean up stack.
    lua_pop(pp->state, lua_gettop(pp->state) - tbl);

    // New table is now at the top of the stack.
}

// Get a reference to the function declared in the parser.  We can't
// include "parser.h" because it conflicts with the Windows headers, which
// are included by dirent.portable.h.
extern void handle_pp_lua_print_line(const char* text, void* ud);

// Get a reference to the function that allows use to push an
// expression as a Lua object.
extern int luaX_pushexpression(lua_State* L, struct expr* expr);

void pp_lua_handle(struct pp_hooks* state, void* scanner, bstring name, list_t* parameters)
{
    struct lua_preproc* pp;
    parameter_t* carg;
    char* cstr;
    unsigned int i;
    int paramtbl;

    // Convert the name to lowercase.
    btolower(name);
    cstr = bstr2cstr(name, '0');

    // Loop through all of the modules.
    list_iterator_start(&modules);
    while (list_iterator_hasnext(&modules))
    {
        pp = list_iterator_next(&modules);

        // Set stack top (I don't know why the top of the
        // stack is negative!)
        lua_settop(pp->state, 0);

        // Search handler table for entries.
        lua_getglobal(pp->state, HANDLER_TABLE_NAME);
        lua_getfield(pp->state, -1, cstr);
        if (!lua_istable(pp->state, -1))
        {
            // No such entry.
            lua_pop(pp->state, 2);
            continue;
        }

        // Call the handler function.
        lua_getfield(pp->state, -1, HANDLER_FIELD_FUNCTION_NAME);
        pp_lua_push_state(pp, state, scanner);
        lua_newtable(pp->state);
        paramtbl = lua_gettop(pp->state);
        for (i = 0; i < list_size(parameters); i++)
        {
            carg = list_get_at(parameters, i);

            lua_newtable(pp->state);
            if (carg->type == EXPRESSION)
                lua_pushstring(pp->state, "EXPRESSION");
            else if (carg->type == ANGLED_STRING)
                lua_pushstring(pp->state, "ANGLED_STRING");
            else if (carg->type == STRING)
                lua_pushstring(pp->state, "STRING");
            else if (carg->type == _WORD)
                lua_pushstring(pp->state, "WORD");
            else
                lua_pushstring(pp->state, "NUMBER");
            lua_setfield(pp->state, -2, "type");
            if (carg->type == EXPRESSION)
                luaX_pushexpression(pp->state, carg->expr);
            else if (carg->type == ANGLED_STRING || carg->type == STRING)
                lua_pushstring(pp->state, carg->string->data);
            else if (carg->type == _WORD)
                lua_pushstring(pp->state, carg->string->data);
            else
                lua_pushnumber(pp->state, carg->number);
            lua_setfield(pp->state, -2, "value");
            lua_rawseti(pp->state, paramtbl, i + 1);
        }
        if (lua_pcall(pp->state, 2, 0, 0) != 0)
        {
            printd(LEVEL_ERROR, "error: unable to call preprocessor handler for '%s'.\n", name->data);
            printd(LEVEL_ERROR, "%s\n", lua_tostring(pp->state, -1));
            bdestroy(name);
            bcstrfree(cstr);
            lua_pop(pp->state, 2);
            list_iterator_stop(&modules);
            list_destroy(parameters);
            return;
        }

        bdestroy(name);
        bcstrfree(cstr);
        lua_pop(pp->state, 2);
        list_iterator_stop(&modules);
        list_destroy(parameters);
        return;
    }
    list_iterator_stop(&modules);

    // Clean up.
    list_destroy(parameters);
    bdestroy(name);
    bcstrfree(cstr);
}


///
/// @endif
///
/// @}
///
