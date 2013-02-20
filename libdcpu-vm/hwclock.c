///
/// @addtogroup LibDCPU-VM
/// @{
///
/// @file
/// @brief  Implementation of generic clock hardware.
/// @sa     http://dcpu.com/clock/
/// @author James Rhodes
/// @author Jose Manuel Diez
///

#define PRIVATE_VM_ACCESS

#include <stdlib.h>
#include <debug.h>

#include "dcpuhook.h"
#include "hwclock.h"

void vm_hw_clock_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;
    (void)pos;
    
    // if clock is enabled:
    if (hw->cylces_per_tick != 0)
    {
        // count cycles
        hw->clock_cycles++;
        
        // if we have reached enough cycles, do a tick
        if (hw->clock_cycles >= hw->cylces_per_tick)
        {
            hw->clock_ticks++;
            hw->clock_cycles = 0;
            
            // if there is a non-zero interrupt message, issue an interrupt
            if (hw->message != 0)
            {
                vm_interrupt(vm, hw->message);
            }
        }
    }
}

void vm_hw_clock_interrupt(vm_t* vm, void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;

    switch (vm->registers[REG_A])
    {
        case TIMER_SET_ENABLED:
            // set cycles per tick, if B==0, then this will also be zero
            hw->cylces_per_tick = (DCPU_TICKS_KHZ * 1000 * vm->registers[REG_B]) / 60;
            hw->clock_ticks = 0;
            hw->clock_cycles = 0;
            vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
            break;

        case TIMER_GET_ELAPSED:
            vm->registers[REG_C] = hw->clock_ticks;
            break;

        case TIMER_SET_INTERRUPT:
            hw->message = vm->registers[REG_B];
            vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
            break;
    }
}

void vm_hw_clock_init(vm_t* vm)
{
    struct clock_hardware* hw;

    hw = malloc(sizeof(struct clock_hardware));
    hw->cylces_per_tick = 0;
    hw->clock_ticks = 0;
    hw->clock_cycles = 0;
    hw->message = 0;
    hw->vm = vm;

    hw->device.id = CLOCK_HARDWARE_ID;
    hw->device.version = CLOCK_VERSION;
    hw->device.manufacturer = CLOCK_MANUFACTURER;
    hw->device.handler = &vm_hw_clock_interrupt;
    hw->device.free_handler = &vm_hw_clock_free;
    hw->device.userdata = hw;

    hw->hook_id = vm_hook_register(vm, &vm_hw_clock_cycle, HOOK_ON_PRE_CYCLE, hw);
    hw->hw_id = vm_hw_register(vm, &hw->device);

    vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
}

void vm_hw_clock_free(void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;

    vm_hook_unregister(hw->vm, hw->hook_id);
    free(hw);
}

///
/// @}
///
