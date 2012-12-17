///
/// @addtogroup LibDCPU-VM
/// @{
///
/// @file
/// @brief Implements the vm_execute function.
/// @author James Rhodes
/// @author Patrick Flick
/// @author Jose Manuel Diez
/// 
/// This implements the functions found in dcpuexec.h.
///

#include <debug.h>

#include "vm.h"
#include "timing.h"

///
/// @brief Actively executes a virtual machine.
///
/// This high-level function actively executes a virtual machine, optionally
/// dumping the virtual machine execution to file.  This function blocks until
/// the virtual machine is halted or exited (caused by either an invalid opcode
/// or debugger hooks).
///
/// This function does not return until that case, so be sure that you have set
/// up an appropriate hook using the functions in dcpuhook.h if you wish to be
/// able to stop or pause the virtual machine whenever desired.
///
/// @param vm The virtual machine to execute.
/// @param execution_dump The filename to dump execution to, or NULL.
///
void vm_execute(vm_t* vm, const char* execution_dump)
{
    struct vm_tod_timer freq_timer;
    struct vm_tod_timer cycle_timer;
    int cycle_count = 0;
    long long freq_cycle_count = 0;
    int sleep_mics = 0;

    if (execution_dump != NULL)
        vm->dump = fopen(execution_dump, "w");

    // Execute the memory using DCPU-16 specifications.
    vm_timing_start_timer(&freq_timer);
    vm_timing_start_timer(&cycle_timer);
    while ((!vm->halted) && (!vm->exit))
    {
        if (vm_timing_has_reached_mics(&freq_timer, 1000000))
        {
            printd(LEVEL_DEBUG, "current speed:    %lld kHz\n", freq_cycle_count / 1000);
            vm_timing_reset_timer(&freq_timer);
            freq_cycle_count = 0;
        }

        vm_cycle(vm);
        freq_cycle_count++;
        cycle_count++;

        if (cycle_count == DCPU_NUM_TIMING_TICKS)
        {
            // setting to 950 instead of 1000 to account for the timing overhead
            // of approx 50 mics
            sleep_mics += (int)(DCPU_NUM_TIMING_TICKS * DCPU_MICS_PER_CYCLE - 50 - vm_timing_get_cur_elapsed_mics(&cycle_timer));
            cycle_count = 0;
            if (sleep_mics > 0)
            {
                vm_timing_sleep_microseconds(sleep_mics);
                // set sleep counter to negative value
                // (because of sleeping too long with usleep)
                sleep_mics = (int)(DCPU_NUM_TIMING_TICKS * DCPU_MICS_PER_CYCLE - vm_timing_get_cur_elapsed_mics(&cycle_timer));
            }
            vm_timing_reset_timer(&cycle_timer);
        }
    }

    if (vm->dump != NULL)
    {
        fclose(vm->dump);
        vm->dump = NULL;
    }
}

///
/// @}
///
