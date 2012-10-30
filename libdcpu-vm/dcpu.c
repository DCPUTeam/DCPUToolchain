/**

    File:       dcpu.c

    Project:        DCPU-16 Tools
    Component:      LibDCPU-vm

    Authors:        James Rhodes
            Patrick Flick
            Jose Manuel Diez

    Description:    Handles high-level operations performed
            on the virtual machine (such as creation).

**/

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

void vm_free(vm_t* vm)
{
    // Free the memory.
    free(vm);
}

void vm_flash(vm_t* vm, uint16_t memory[0x10000])
{
    // Flash the VM's memory from the specified array.
    unsigned int i;
    vm_init(vm, false);

    for (i = 0; i < 0x10000; i++)
        vm->ram[i] = memory[i];
}

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
            printf("current speed:    %lld kHz\n", freq_cycle_count / 1000);
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
