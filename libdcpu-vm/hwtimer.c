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

// Hook callback triggered each VM cycle.
void vm_hw_clock_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;
    (void)pos;

    if (hw->message != 0)
    {
        if (hw->clock_ticks >= hw->clock_target)
        {
            vm_interrupt(vm, hw->message);
            hw->clock_ticks = 0;
        }
        else
            hw->clock_ticks++;
    }
}

// Interrupt callback, triggered by the HWI instruction.
void vm_hw_clock_interrupt(vm_t* vm, void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;

    switch (vm->registers[REG_A])
    {
        case TIMER_SET_ENABLED:
            if (vm->registers[REG_B] == 0x0)
                break;
            else
            {
                if (vm->registers[REG_B] > 60) break;
                hw->clock_target = (DCPU_TICKS_KHZ * 1000 * vm->registers[REG_B]) / 60;
                vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
            }

            break;

        case TIMER_GET_ELAPSED:
            vm->registers[REG_C] = hw->clock_ticks;
            hw->clock_ticks = 0;
            break;

        case TIMER_SET_INTERRUPT:
            hw->message = vm->registers[REG_B];
            vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
            break;
    }
}

// Add a clock hardware to the given VM.
void vm_hw_clock_init(vm_t* vm)
{
    struct clock_hardware* hw;

    hw = malloc(sizeof(struct clock_hardware));
    hw->clock_target = 0;
    hw->clock_ticks = 0;
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

// Callback that frees the clock hardware.
void vm_hw_clock_free(void* ud)
{
    struct clock_hardware* hw = (struct clock_hardware*) ud;

    vm_hook_unregister(hw->vm, hw->hook_id);
    free(hw);
}


///
/// @}
///