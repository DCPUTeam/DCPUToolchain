///
/// @addtogroup LibDCPU-VM
/// @{
///
/// @file
/// @brief  Declares macros and functions used to host the interrupt clock (generic clock).
/// @sa     http://dcpu.com/clock/
/// @author James Rhodes
///

#ifndef __DCPU_HWCLOCK_H
#define __DCPU_HWCLOCK_H

#include "hw.h"

/// The struct that holds the information for the clock hardware.
struct clock_hardware
{
    hw_t device;
    vm_t* vm;

    uint16_t hook_id;
    uint16_t hw_id;

    uint32_t clock_target;
    uint32_t clock_ticks;

    uint16_t message;
};

/// defines the hardware ID of the generic clock
#define CLOCK_HARDWARE_ID       0x12D0B402
/// defines the version number of the generic clock
#define CLOCK_VERSION           0x0001
/// defines the manufacturer ID of the generic clock
#define CLOCK_MANUFACTURER      0x00000000

// internal defines to be used as states
#define TIMER_SET_ENABLED   0
#define TIMER_GET_ELAPSED   1
#define TIMER_SET_INTERRUPT 2


///
/// @brief Add a clock hardware to the given VM.
///
/// @param vm The virutal machine to install the hardware on.
///
void vm_hw_clock_init(vm_t* vm);


///
/// @brief Hook callback triggered each VM cycle.
///
/// @param vm The virtual machine where the hook was fired from.
/// @param pos Not used.
/// @param ud The clock that the interrupt was sent to.
///
void vm_hw_clock_cycle(vm_t* vm, uint16_t pos, void* ud);


///
/// @brief Interrupt callback, triggered by the HWI instruction.
///
/// @param vm The virtual machine where the interrupt was fired from.
/// @param ud The clock that the interrupt was sent to
///
void vm_hw_clock_interrupt(vm_t* vm, void* ud);

///
/// @brief Callback that frees the clock hardware.
///
/// @param ud The hw to free that was given as a userdata.
///
void vm_hw_clock_free(void* ud);

#endif

///
/// @}
///
