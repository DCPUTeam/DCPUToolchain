/**

    File:	    hwsped3.h

    Project:	    DCPU-16 Tools
    Component:	    LibDCPU-vm

    Authors:	    Jose Manuel Diez
		    David Herberth
		    Patrick Flick

    Description:    Function signatures and sped-3 modes.
**/

#ifndef __DCPUHWSPED3_H
#define __DCPUHWSPED3_H

#include <GL/glfw3.h>
#include "dcpu.h"

#define SPED3_ID	    0x42babf3c
#define SPED3_VERSION	    0x0003
#define SPED3_MANUFACTURER  0x1eb37e91

#define SPED3_INTERRUPT_POLL   0
#define SPED3_INTERRUPT_MAP    1
#define SPED3_INTERRUPT_ROTATE 2

#define SPED3_STATE_NO_DATA 0
#define SPED3_STATE_RUNNING 1
#define SPED3_STATE_TURNING 2
#define SPED3_STATE_QUIT_WINDOW 3

#define SPED3_ERROR_NONE    0
#define SPED3_ERROR_BROKEN  0xffff

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

struct sped3_hardware
{
    // this struct has to be in the first position
    // it is used to distinguish between the GLFW windows
    hw_t device;

    uint16_t cycle_hook;
    uint16_t hw_id;

    void* context;
    int width, height;

    uint16_t mem;
    uint16_t num;
    double rot_target;
    double rot_current;

    uint16_t state;
    vm_t* vm;
    
    int window_closed;
};

void vm_hw_sped3_init(vm_t* vm);
void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_sped3_free(void* ud);
int vm_hw_sped3_close(GLFWwindow w);

#endif
