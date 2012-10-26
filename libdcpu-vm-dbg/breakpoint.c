/**

    File:       breakpoint.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-VM-DBG

    Authors:    James Rhodes

    Description:    Defines breakpoint functionality.

**/

#include <stdlib.h>
#include <simclist.h>
#include <dcpu.h>
#include "breakpoint.h"

size_t breakpoint_meter(const void* el)
{
    return sizeof(struct breakpoint);
}

int breakpoint_seeker(const void* el, const void* indicator)
{
    struct breakpoint* bk = (struct breakpoint*)el;
    uint16_t addr = *(uint16_t*)indicator;
    return (bk->addr == addr);
}

struct breakpoint* breakpoint_create(uint16_t addr, bool silent, bool temporary)
{
    struct breakpoint* bk = malloc(sizeof(struct breakpoint));
    bk->addr = addr;
    bk->silent = silent;
    bk->temporary = temporary;
    return bk;
}

list_t breakpoint_list_create()
{
    list_t list;
    list_init(&list);
    list_attributes_copy(&list, &breakpoint_meter, 1);
    list_attributes_seeker(&list, &breakpoint_seeker);
    return list;
}

