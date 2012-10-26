/**

    File:       timing.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    Patrick Flick

    Description:    Declares the timing functions for precise timing of
            the 100 kHz cycle speed.

**/

#ifndef __TIMING_H
#define __TIMING_H


struct vm_tod_timer
{
    long long mics_since_midnight_start;
    long long mics_since_midnight_end;
};

long long vm_timing_microsecsonds_since_midnight_GMT();
void vm_timing_start_timer(struct vm_tod_timer* timer);
int vm_timing_has_reached_mics(struct vm_tod_timer* timer, long long mics);
void vm_timing_reset_timer(struct vm_tod_timer* timer);
void vm_timing_stop_timer(struct vm_tod_timer* timer);
long long vm_timing_get_elapsed_mics(struct vm_tod_timer* timer);
long long vm_timing_get_cur_elapsed_mics(struct vm_tod_timer* timer);
void vm_timing_sleep_microseconds(unsigned int microseconds);

#endif
