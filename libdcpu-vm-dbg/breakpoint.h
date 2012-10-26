/**

    File:       breakpoint.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-VM-DBG

    Authors:    James Rhodes

    Description:    Declares breakpoint structures.

**/

#ifndef _DCPU_DBG_BREAKPOINT
#define _DCPU_DBG_BREAKPOINT

#include <simclist.h>
#include <dcpu.h>

struct breakpoint
{
    uint16_t addr;
    bool silent;
    bool temporary;
};

struct breakpoint* breakpoint_create(uint16_t addr, bool silent, bool temporary);
list_t breakpoint_list_create();

#endif
