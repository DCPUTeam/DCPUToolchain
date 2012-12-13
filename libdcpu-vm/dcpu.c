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
/// This function is ment to be used when you have embedded the vm_t
/// in your own struct and you don't want to use vm_create. Note when
/// have embedded it you must call this function to set pointers into
/// a valid state, if you have created vm_t via vm_create and added
/// hardware you must @b NOT call this function, since this function
/// assumes that vm_t is in a unkown state. Use vm_reset for resetting
/// the vm without removing the hardware.
///
/// @param vm The virtual machine structure to initialize.
///
void vm_init(vm_t* vm)
{
    // Since we use the big memset hammer here, a lot of the
    // setting we do is not needed it is more for bookkeeping.

    // Rule 2, double tap.
    memset(vm, 0, sizeof(*vm));

    // Some fields isn't supposed to be Zero.
    vm_reset(vm, false);

    // This should only be done on init.
    vm->host = NULL;
    vm->debug = false;

    // Init subsystem that hangs on the vm struct.
    vm_hook_initialize(vm);
}

///
/// @brief Resets a virtual machine structure.
///
/// This function is intended to be used when reseting the hardware back
/// to initial state on a vm_t struct created with vm_create or initialized
/// with vm_init first. It does not remove hardware nor any installed hooks.
/// All interrupts are removed from the interrupt queue tho.
///
/// @param vm The virtual machine structure to reset.
/// @param reset_memory Whether to wipe the virtual machine's memory.
///
void vm_reset(vm_t* vm, bool reset_memory)
{
    unsigned int i;

    for (i = 0; i < 0x8; i++)
        vm->registers[i] = 0x0;

    vm->pc = 0x0;
    vm->sp = 0x0;
    vm->ex = 0x0;
    vm->ia = 0x0;

    if (reset_memory)
    {
        for (i = 0; i < 0x10000; i++)
            vm->ram[i] = 0x0;
    }

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
    vm_init(new_vm);

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
/// integers also calls vm_reset on the virtual machine.
///
/// @param vm The virtual machine whose memory should be flashed.
/// @param memory The unsigned 16-bit integers to flash the memory with.
///
void vm_flash(vm_t* vm, uint16_t memory[0x10000])
{
    // Flash the VM's memory from the specified array.
    unsigned int i;
    vm_reset(vm, false);

    for (i = 0; i < 0x10000; i++)
        vm->ram[i] = memory[i];
}

///
/// @}
///
