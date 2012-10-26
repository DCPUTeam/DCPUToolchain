/**

    File:    hwm35fd.h

    Project:    DCPU-16 Tools
    Component:    LibDCPU-vm

    Authors:    Jose Manuel Diez

    Description: Function signatures and m35fd modes.
**/

#ifndef __DCPUHWM35FD_H
#define __DCPUHWM35FD_H

#include "dcpu.h"

#define M35FD_STATE_NO_MEDIA     0
#define M35FD_STATE_READY     1
#define M35FD_STATE_READY_WP     2
#define M35FD_STATE_READING 3
#define M35FD_STATE_WRITING 4

#define M35FD_ERROR_NONE     0
#define M35FD_ERROR_BUSY     1
#define M35FD_ERROR_NO_MEDIA     2
#define M35FD_ERROR_PROTECTED     3
#define M35FD_ERROR_EJECT     4
#define M35FD_ERROR_BAD_SECTOR     5
#define M35FD_ERROR_BROKEN     0xffff

#define M35FD_INTERRUPT_POLL     0
#define M35FD_INTERRUPT_MSG 1
#define M35FD_INTERRUPT_READ     2
#define M35FD_INTERRUPT_WRITE     3

struct m35fd_hardware
{
    hw_t device;

    uint16_t hw_hook;
    uint16_t hw_id;
    uint16_t interrupt_msg;

    uint16_t state;
    uint16_t last_error;

    FILE* disk;
    uint16_t sector;
    uint16_t position;

    bool reading;
    bool writing;
    bool write_protected;

    vm_t* vm;
};

void vm_hw_m35fd_init(vm_t* vm);
void vm_hw_m35fd_cycle(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_m35fd_free(void* ud);

#endif
