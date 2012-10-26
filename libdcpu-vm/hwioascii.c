/**

    File:       hwioascii.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    James Rhodes
            Jose Manuel Diez

    Description:    Defines the vm_hw_io_ascii_get_map function which
            maps glfw values to the ASCII equivalents.

**/

#include "hwioascii.h"

uint16_t vm_hw_io_ascii_get_map(int key)
{
    uint16_t ascii = key;
    return ascii;
}
