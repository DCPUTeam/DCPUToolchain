/**

    File:       hw.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    José Manuel Díez

    Description:    Defines the type which holds a hardware device,
            provides a mechanism to call interrupts on them

**/

#ifndef __DCPUHW_H
#define __DCPUHW_H

#include "vm.h"

#ifdef PRIVATE_VM_ACCESS
void vm_hw_initialize(vm_t* vm);
void vm_hw_free_all(vm_t* vm);
#endif

uint16_t vm_hw_register(vm_t* vm, hw_t* device);
uint16_t vm_hw_count(vm_t* vm);
hw_t* vm_hw_get_device(vm_t* vm, uint16_t index);
void vm_hw_interrupt(vm_t* vm, uint16_t index);

#endif
