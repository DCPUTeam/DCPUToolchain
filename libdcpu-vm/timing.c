/**

    File:       dcpu.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    Patrick Flick

    Description:    Implements the timing functions for precise timing of
            the 100 kHz cycle speed.

**/

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#else
// TODO windows compatible timing functions
#endif

#include "timing.h"
#include <stdlib.h>


long long vm_timing_microsecsonds_since_midnight_GMT()
{
#ifndef WIN32
    struct timeval t;
    gettimeofday(&t, NULL);
    return((long long)t.tv_sec % (24 * 3600) * 1000000 + t.tv_usec);
#else
    // TODO
    return 0;
#endif
}

void vm_timing_start_timer(struct vm_tod_timer* timer)
{
    timer->mics_since_midnight_start = vm_timing_microsecsonds_since_midnight_GMT();
}

int vm_timing_has_reached_mics(struct vm_tod_timer* timer, long long mics)
{
    long long elapsed = vm_timing_get_cur_elapsed_mics(timer);
    return (elapsed >= mics);
}

void vm_timing_reset_timer(struct vm_tod_timer* timer)
{
    // same as start timer:
    timer->mics_since_midnight_start = vm_timing_microsecsonds_since_midnight_GMT();
}

void vm_timing_stop_timer(struct vm_tod_timer* timer)
{
    timer->mics_since_midnight_end = vm_timing_microsecsonds_since_midnight_GMT();
}

long long vm_timing_get_elapsed_mics(struct vm_tod_timer* timer)
{
    if (timer->mics_since_midnight_end < timer->mics_since_midnight_start)
    {
        timer->mics_since_midnight_end += 24ll * 3600ll * 1000000ll;
    }
    return (timer->mics_since_midnight_end - timer->mics_since_midnight_start);
}

long long vm_timing_get_cur_elapsed_mics(struct vm_tod_timer* timer)
{
    long long cur_time;
    long long elapsed;
    cur_time = vm_timing_microsecsonds_since_midnight_GMT();
    if (cur_time < timer->mics_since_midnight_start)
    {
        cur_time += 24ll * 3600ll * 1000000ll;
    }
    elapsed = cur_time - timer->mics_since_midnight_start;
    return elapsed;
}

void vm_timing_sleep_microseconds(unsigned int microseconds)
{
#ifndef WIN32
    usleep(microseconds);
#else
    // TODO
#endif
}

