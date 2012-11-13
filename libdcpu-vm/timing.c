/**

    File:           dcpu.c

    Project:        DCPU-16 Tools
    Component:      LibDCPU-vm

    Authors:        Patrick Flick

    Description:    Implements the timing functions for precise timing of
                    the 100 kHz cycle speed.

**/

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "timing.h"
#include <stdlib.h>


#ifdef WIN32
// whether the timer has been initialized (which has to happen)
int vm_timing_windows_timer_initialized = 0;
//resolution
double vm_timing_windows_thread_time_res = 0.0;
//initial time
unsigned __int64 vm_timing_thread_time64 = 0;
#endif


long long vm_timing_microsecsonds_since_midnight_GMT()
{
#ifndef WIN32
    struct timeval t;
    gettimeofday(&t, NULL);
    return((long long)t.tv_sec % (24 * 3600) * 1000000 + t.tv_usec);
#else
    unsigned __int64 frequency;
    unsigned __int64 t_64;
    if (vm_timing_windows_timer_initialized == 0)
    {
        // init timer before returning 
        if (QueryPerformanceFrequency((LARGE_INTEGER*)&frequency)) {
            vm_timing_windows_thread_time_res = 1.0 / (double)frequency;
            vm_timing_windows_timer_initialized = 1;
            QueryPerformanceCounter((LARGE_INTEGER*)&vm_timing_thread_time64);
        }
    }
    if (QueryPerformanceCounter((LARGE_INTEGER*)&t_64)) {
        return (long long)((double)(t_64 - vm_timing_thread_time64)*vm_timing_windows_thread_time_res);
    } else {
        return 0;
    }
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
    Sleep(microseconds/1000u);
#endif
}

