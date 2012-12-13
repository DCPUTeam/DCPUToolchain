/**

    File:       hw.c

    Project:        DCPU-16 Tools
    Component:      LibDCPU-vm

    Authors:        José Manuel Díez
            Patrick Flick

    Description:    Handles opcode instructions in the
            virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdlib.h>
#include <stdio.h>
#include <debug.h>
#include "dcpubase.h"
#include "hw.h"

void vm_hw_initialize(vm_t* vm)
{
    int i;
    for (i = 0; i < VM_HW_MAX; i++)
    {
        vm->hw_list[i] = 0;
    }
}

uint16_t vm_hw_register(vm_t* vm, hw_t* hardware)
{
    uint16_t id = 0;

    while (vm->hw_list[id] != NULL && id < VM_HW_MAX)
        id++;

    if (id >= VM_HW_MAX)
    {
        vm_halt(vm, "unable to register hardware, maximum reached!");
        return 0;
    }

    printd(LEVEL_DEBUG, "assigned id %d: 0x%08X\n", id, hardware->id);
    vm->hw_list[id] = hardware;

    return id;
}

void vm_hw_interrupt(vm_t* vm, uint16_t index)
{
    if (index < VM_HW_MAX)
    {
        hw_t* device = vm->hw_list[index];
        if (device == NULL)
            return; // Halt?

        if (vm->debug) printd(LEVEL_DEBUG, "\nInterrupting device 0x%04X (0x%08X): %p\n", index, device->id, device->handler);

        if (device->handler != NULL)
            device->handler(vm, device->userdata);
    }
}

uint16_t vm_hw_count(vm_t* vm)
{
    uint16_t i = 0;

    for (i = 0; i < VM_HW_MAX; i++)
    {
        if (vm->hw_list[i] == NULL)
            return i;
    }

    return VM_HW_MAX;
}

hw_t* vm_hw_get_device(vm_t* vm, uint16_t index)
{
    return vm->hw_list[index];
}

void vm_hw_free_all(vm_t* vm)
{
    uint16_t id = 0;
    hw_t* hw;

    do {
        hw = vm->hw_list[id];
        vm->hw_list[id++] == NULL;

        if (hw == NULL)
            continue;

        if (hw->free_handler != NULL)
            hw->free_handler(hw->userdata);

    } while(id < VM_HW_MAX);
}
