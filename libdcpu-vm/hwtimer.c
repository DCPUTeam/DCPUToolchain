///
/// @addtogroup LibDCPU-VM
/// @{
///
/// @file
/// @brief Implementation of timer hardware.
/// @author James Rhodes
/// @author Jose Manuel Diez
///

#define PRIVATE_VM_ACCESS

#include <stdlib.h>
#include <debug.h>

#include "dcpuhook.h"
#include "hwtimer.h"

///
/// @brief Hook callback triggered each VM cycle.
///
/// @param vm The virtual machine where the hook was fired from.
/// @param pos Not used.
/// @param ud The timer that the interrupt was sent to.
///
void vm_hw_timer_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct timer_hardware* hw = (struct timer_hardware*) ud;
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

///
/// @brief Interrupt callback, triggered by the HWI instruction.
///
/// @param vm The virtual machine where the interrupt was fired from.
/// @param ud The timer that the interrupt was sent to
///
void vm_hw_timer_interrupt(vm_t* vm, void* ud)
{
    struct timer_hardware* hw = (struct timer_hardware*) ud;

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

///
/// @brief Add a timer hardware to the given VM.
///
/// @param vm The virutal machine to install the hardware on.
///
void vm_hw_timer_init(vm_t* vm)
{
    struct timer_hardware* hw;

    hw = malloc(sizeof(struct timer_hardware));
    hw->clock_target = 0;
    hw->clock_ticks = 0;
    hw->message = 0;
    hw->vm = vm;

    hw->device.id = 0x12D0B402;
    hw->device.version = 0x0001;
    hw->device.manufacturer = 0x00000000;
    hw->device.handler = &vm_hw_timer_interrupt;
    hw->device.free_handler = &vm_hw_timer_free;
    hw->device.userdata = hw;

    hw->hook_id = vm_hook_register(vm, &vm_hw_timer_cycle, HOOK_ON_PRE_CYCLE, hw);
    hw->hw_id = vm_hw_register(vm, &hw->device);

    vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
}

///
/// @brief Callback that frees the timer hardware.
///
/// @param ud The hw to free that was given as a userdata.
///
void vm_hw_timer_free(void* ud)
{
    struct timer_hardware* hw = (struct timer_hardware*) ud;

    vm_hook_unregister(hw->vm, hw->hook_id);
    free(hw);
}
