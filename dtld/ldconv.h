/**

    File:       ldconv.h

    Project:    DCPU-16 Toolchain
    Component:  Linker

    Authors:    James Rhodes

    Description:    Converts object format structures to linked lists
            so that they can be easily modified and cloned.

**/

#ifndef __DCPU_LD_CONV_H
#define __DCPU_LD_CONV_H

#include <bstring.h>
#include <simclist.h>
#include <lprov.h>

struct lconv_entry
{
    bstring label;
    bstring bin;
    uint16_t address;
};

list_t* list_create();
list_t* list_convert(struct lprov_entry* first);
list_t* list_clone(list_t* original);
struct lprov_entry* list_revert(list_t* list);
void list_prov_destroy(list_t* list);

#endif