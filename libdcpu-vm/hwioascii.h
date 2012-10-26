/**

    File:       hwioascii.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    James Rhodes

    Description:    Declares the vm_hw_io_ascii_get_map function which
            maps libTCOD values to the ASCII equivalents.

**/

#ifndef __DCPUHWIOASCII_H
#define __DCPUHWIOASCII_H

#include <stdint.h>

uint16_t vm_hw_io_ascii_get_map(int key);

#endif
