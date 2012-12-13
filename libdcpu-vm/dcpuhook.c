/**

    File:       dcpuops.c

    Project:    DCPU-16 Tools
    Component:  Emulator

    Authors:    James Rhodes
            Aaron Miller
            Jose Manuel Diez

    Description:    Handles opcode instructions in the
            virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include "dcpubase.h"
#include "dcpuhook.h"


void vm_hook_fire(vm_t* vm, uint16_t pos, uint16_t mode, void* ud)
{
    uint16_t i;

    for (i = 0; i < VM_HOOK_MAX; i += 1)
        if (vm->hook_list[i] != NULL && vm->hook_mode[i] == mode)
            vm->hook_list[i](vm, pos, ud == NULL ? vm->hook_userdata[i] : ud);
}

void vm_hook_break(vm_t* vm)
{
    vm_hook_fire(vm, 0, HOOK_ON_BREAK, NULL);
}

void vm_hook_initialize(vm_t* vm)
{
    int i;
    for (i = 0; i < VM_HOOK_MAX; i++)
    {
        vm->hook_list[i] = NULL;
        vm->hook_mode[i] = 0;
        vm->hook_userdata[i] = NULL;
    }
}

uint16_t vm_hook_register(vm_t* vm, vm_hook hook, uint16_t mode, void* ud)
{
    uint16_t id = 0;

    printd(LEVEL_EVERYTHING, "registering hook\n");

    while (vm->hook_list[id] != NULL && id < VM_HOOK_MAX)
        id++;

    if (id >= VM_HOOK_MAX)
    {
        vm_halt(vm, "unable to register hook, maximum reached!\n");
        return 0;
    }

    vm->hook_list[id] = hook;
    vm->hook_mode[id] = mode;
    vm->hook_userdata[id] = ud;
    return id;
}

void vm_hook_unregister(vm_t* vm, uint16_t id)
{
    vm->hook_list[id] = NULL;
    vm->hook_mode[id] = HOOK_ON_NONE;
    vm->hook_userdata[id] = NULL;
}
