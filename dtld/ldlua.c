/**

    File:       ldlua.c

    Project:    DCPU-16 Toolchain
    Component:  Linker

    Authors:    James Rhodes

    Description:    Provides optimization infrastructure for Lua-based
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
#include <hw.h>
#include <hwluacpu.h>
#include <debug.h>
#include <osutil.h>
#include <ppexprlua.h>
#include <luaglob.h>
#include <derr.h>
#include "ldlua.h"
#include "ldconv.h"
#include "ldbins.h"

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
    // FIXME: This doesn't have a valid value for some reason (it's blank).
    om->name = bstrcpy(name);
    om->state = lua_open();
    assert(om->state != NULL);
    luaL_openlibs(om->state);
    luaX_loadexpressionlib(om->state);

    // Load globals.
    dcpu_lua_set_constants(om->state);

    // Execute the code in the new Lua context.
    if (luaL_dofile(om->state, path->data) != 0)
    {
        const char* lerr = lua_tostring(om->state, -1);

        // Return NULL.
        lua_close(om->state);
        bdestroy(om->name);
        free(om);
        bdestroy(path);
        dhalt(ERR_LUA, lerr);
        return NULL;
    }

    // Load tables.
    lua_getglobal(om->state, "MODULE");
    module = lua_gettop(om->state);

    // Ensure module table was provided.
    if (lua_isnoneornil(om->state, module))
    {
        // Return NULL.
        lua_close(om->state);
        bdestroy(om->name);
        free(om);
        dhalt(ERR_OPTIMIZER_LOAD_FAILED, path->data);
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
        bdestroy(om->name);
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

struct ldbin* bin_lua_extract_bin(lua_State* L, int idx)
{
    struct ldbin* bin = NULL;
    lua_getmetatable(L, idx);
    lua_rawgeti(L, lua_gettop(L), 0);
    if (!lua_isuserdata(L, -1))
    {
        lua_pushstring(L, "state must be first parameter to function, or use : operator.");
        lua_error(L);
    }
    bin = (struct ldbin*)lua_touserdata(L, -1);
    lua_pop(L, 2);
    return bin;
}

vm_t* bin_lua_extract_vm(lua_State* L, int idx)
{
    vm_t* vm = NULL;
    lua_getmetatable(L, idx);
    lua_rawgeti(L, lua_gettop(L), 1);
    if (!lua_isuserdata(L, -1))
    {
        lua_pushstring(L, "state must be first parameter to function, or use : operator.");
        lua_error(L);
    }
    vm = lua_touserdata(L, -1);
    lua_pop(L, 2);
    return vm;
}

int bin_lua_reflash(lua_State* L)
{
    size_t i;
    struct ldbin* bin = bin_lua_extract_bin(L, 1);
    vm_t* vm = bin_lua_extract_vm(L, 1);
    vm_init(vm, true);
    for (i = 0; i < list_size(&bin->words); i++)
        vm->ram[i] = *(uint16_t*)(list_get_at(&bin->words, i));
    return 0;
}

int bin_lua_remove(lua_State* L)
{
    size_t i;
    struct ldbin* bin = bin_lua_extract_bin(L, 1);
    vm_t* vm = bin_lua_extract_vm(L, 1);
    uint16_t offset = (uint16_t)luaL_checknumber(L, 2);
    uint16_t count = (uint16_t)luaL_optnumber(L, 3, 1);
    if (offset + count >= 0 && (uint32_t)offset + (uint32_t)count <= (uint32_t)list_size(&bin->words))
    {
        // Remove words.
        bin_remove(&ldbins.bins, bin, offset, count);

        // Implicit reflash.
        vm_init(vm, true);
        for (i = 0; i < list_size(&bin->words); i++)
            vm->ram[i] = *(uint16_t*)(list_get_at(&bin->words, i));
    }
    else
        luaL_error(L, "invalid offset / count parameters for :remove");
    return 0;
}

void bin_lua_push_bin_list(lua_State* L, list_t* list)
{
    struct lconv_entry* entry;
    size_t i;

    // Create the new table.
    lua_newtable(L);

    // Check if list is NULL.
    if (list == NULL)
        return; // Empty table on top of stack.

    // Fill table.
    for (i = 0; i < list_size(list); i++)
    {
        entry = list_get_at(list, i);

        // Create entry.
        lua_newtable(L);
        lua_pushstring(L, "address");
        lua_pushnumber(L, entry->address);
        lua_rawset(L, -3);
        lua_pushstring(L, "label");
        if (entry->label == NULL)
            lua_pushnil(L);
        else
            lua_pushstring(L, entry->label->data);
        lua_rawset(L, -3);

        // Add entry.
        lua_rawseti(L, -2, i + 1);
    }

    // Table is now on top of the stack.
}

void bin_lua_push_state(lua_State* L, struct ldbin* bin)
{
    int tbl, tbl_mt;
    vm_t* vm;
    size_t i;

    // Create the new table and metatable.
    lua_newtable(L);
    tbl = lua_gettop(L);
    lua_newtable(L);
    tbl_mt = lua_gettop(L);

    // Push userdata into metatable.
    lua_pushlightuserdata(L, bin);
    lua_rawseti(L, tbl_mt, 0);
    vm = lua_newuserdata(L, sizeof(vm_t));
    lua_rawseti(L, tbl_mt, 1);

    // Initialize new virtual machine without hardware so that
    // the optimizer can disassemble instructions.
    vm_init(vm, true);
    for (i = 0; i < list_size(&bin->words); i++)
        vm->ram[i] = *(uint16_t*)(list_get_at(&bin->words, i));

    // Push virtual machine.
    vm_hw_lua_cpu_push_cpu(L, vm);
    lua_setfield(L, tbl, "cpu");

    // Push additional information.
    bin_lua_push_bin_list(L, bin->adjustment);
    lua_setfield(L, tbl, "adjustment");
    bin_lua_push_bin_list(L, bin->required);
    lua_setfield(L, tbl, "required");
    bin_lua_push_bin_list(L, bin->provided);
    lua_setfield(L, tbl, "provided");
    bin_lua_push_bin_list(L, bin->section);
    lua_setfield(L, tbl, "section");
    bin_lua_push_bin_list(L, bin->output);
    lua_setfield(L, tbl, "output");
    lua_pushnumber(L, list_size(&bin->words));
    lua_setfield(L, tbl, "wordcount");
    lua_pushcfunction(L, &bin_lua_remove);
    lua_setfield(L, tbl, "remove");
    lua_pushcfunction(L, &bin_lua_reflash);
    lua_setfield(L, tbl, "reflash");

    // Associate metatables.
    lua_pushvalue(L, tbl_mt);
    lua_setmetatable(L, tbl);

    // Clean up stack.
    lua_pop(L, lua_gettop(L) - tbl);

    // New table is now at the top of the stack.
}

///
/// Optimizes a bin using the Lua optimization infrastructure.
///
/// @returns The number of words saved during optimization.
///
int32_t bin_lua_optimize(struct ldbin* bin)
{
    unsigned int i;
    struct lua_optmst* om;
    int32_t size = list_size(&bin->words);

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
        bin_lua_push_state(om->state, bin);
        if (lua_pcall(om->state, 1, 0, 0) != 0)
        {
            printd(LEVEL_ERROR, "error: unable to call optimizer '%s'.\n", om->name);
            printd(LEVEL_ERROR, "%s\n", lua_tostring(om->state, -1));
            lua_pop(om->state, 1);
            continue;
        }

        printd(LEVEL_VERBOSE, "note: called optimizer '%s'.\n", om->name);
    }

    // Return value.
    return size - list_size(&bin->words);
}
