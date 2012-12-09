/**

    File:       osutil.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Declares functions for interacting with the
            local operating system (such as getting
            directory names, etc.)

**/

#ifndef __DCPU_OSUTIL_H
#define __DCPU_OSUTIL_H

#include <stdio.h>
#include <stdint.h>
#include "bstring.h"

struct ostimeval
{
    int32_t tv_sec;
    int32_t tv_usec;
};

bstring osutil_dirname(bstring path);
void osutil_makebinary(FILE* fd);
void osutil_setarg0(freed_bstring arg0);
bstring osutil_getarg0();
bstring osutil_getarg0path();
bstring osutil_getmodulepath();
bstring osutil_getkernelpath();
bstring osutil_getstdlibpath();
const char* osutil_getkerneldefault();
int osutil_gettimeofday(struct ostimeval* tv, void* unused);
void osutil_usleep(int milliseconds);
FILE* osutil_popen(bstring command, bstring type);

#endif
