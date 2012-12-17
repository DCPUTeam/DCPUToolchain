/**

    File:       dcpuhook.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    James Rhodes

    Description:    Declares the hooking mechanism for observing
            when the VM's RAM changes.

**/

#ifndef __DCPUHOOK_H
#define __DCPUHOOK_H

#include "vm.h"

#define HOOK_ON_NONE        0
#define HOOK_ON_WRITE       1
#define HOOK_ON_POST_CYCLE  2
#define HOOK_ON_BREAK       3
#define HOOK_ON_PRE_CYCLE   4
#define HOOK_ON_INTERRUPT   5
#define HOOK_ON_60HZ        6
#define HOOK_ON_HARDWARE_CHANGE 7

#ifdef PRIVATE_VM_ACCESS
void vm_hook_fire(vm_t* vm, uint16_t pos, uint16_t mode, void* ud);
#endif

void vm_hook_initialize(vm_t *vm);
// Mode should be one of the defined values above.
uint16_t vm_hook_register(vm_t* vm, vm_hook hook, uint16_t mode, void* ud);
void vm_hook_unregister(vm_t* vm, uint16_t id);
void vm_hook_break(vm_t* vm);

#endif
