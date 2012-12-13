///
/// @addtogroup LibDCPU
/// @{
///
/// @file
/// @brief General functions and definitions used through-out the toolchain.
/// @author James Rhodes
/// @author Patrick Flick
/// @author Jose Manuel Diez
/// 
/// This implements the supporting functions found in dcpu.h.
///

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <debug.h>
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "hwtimer.h"
#include "hw.h"
#include "timing.h"

///
/// @brief Initializes a new virtual machine structure.
///
/// @param vm The virtual machine structure to initialize.
/// @param init_memory Whether to wipe the virtual machine's memory.
///
void vm_init(vm_t* vm, bool init_memory)
{
    unsigned int i;

    for (i = 0; i < 0x8; i++)
        vm->registers[i] = 0x0;

    vm->pc = 0x0;
    vm->sp = 0x0;
    vm->ex = 0x0;
    vm->ia = 0x0;

    if (init_memory)
    {
        for (i = 0; i < 0x10000; i++)
            vm->ram[i] = 0x0;
    }

    vm->host = NULL;

    vm->sleep_cycles = 0;
    vm->dummy = 0x0;
    vm->halted = false;
    vm->exit = false;
    vm->skip = false;

    vm->can_fire = false;
    vm->on_fire = false;
    vm->fire_cycles = 0;
    vm->fire_cycles_target = 0;

    vm->radiation = false;
    vm->radiation_cycles = 0;
    vm->radiation_cycles_target = 0;
    vm->radiation_factor = 1;

    printd(LEVEL_DEBUG, "turning off interrupt queue\n");
    vm->queue_interrupts = false;
    vm->irq_count = 0;
    vm->dump = NULL;
    for (i = 0; i < 256; i++)
        vm->irq[i] = 0x0;

    return;
}

///
/// @brief Allocates a new virtual machine in memory and initializes it.
///
/// @return The new virtual machine structure.  It must be later freed
///         with vm_free().
///
vm_t* vm_create()
{
    // Define variables.
    vm_t* new_vm;

    // Allocate and wipe vm memory.
    new_vm = (vm_t*)malloc(sizeof(vm_t));
    vm_init(new_vm, true);
    new_vm->debug = false;

    // Return.
    return new_vm;
}

///
/// @brief Frees a virtual machine structure from memory.
///
/// @param vm The virtual machine structure previously allocated with vm_create().
///
void vm_free(vm_t* vm)
{
    // Free the memory.
    free(vm);
}

///
/// @brief Flashes the virtual machine's memory.
///
/// Flashes the virtual machine's memory from the array of unsigned 16-bit
/// integers.
///
/// @param vm The virtual machine whose memory should be flashed.
/// @param memory The unsigned 16-bit integers to flash the memory with.
///
void vm_flash(vm_t* vm, uint16_t memory[0x10000])
{
    // Flash the VM's memory from the specified array.
    unsigned int i;
    vm_init(vm, false);

    for (i = 0; i < 0x10000; i++)
        vm->ram[i] = memory[i];
}

///
/// @}
///
