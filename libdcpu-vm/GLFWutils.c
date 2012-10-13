/**

    File:           GLFWutils.c

    Project:        DCPU-16 Toolchain
    Component:      LibDCPU-VM

    Authors:        Patrick Flick

    Description:    Defines utility functions for glfw.

**/

#include "hwlem1802.h"
#include "hwsped3.h"
#include <GL/glfw3.h>
#include "dcpu.h"


int vm_hw_glfw_close_window_callback(GLFWwindow w)
{
    void* ud;
    hw_t * device;
    uint32_t device_id;
    ud = glfwGetWindowUserPointer(w);
    device = (hw_t*) ud;
    device_id = device->id;
    if (device_id == LEM1802_ID)
    {
        vm_hw_lem1802_close(w);
    }
    else if (device_id == SPED3_ID)
    {
        vm_hw_sped3_close(w);
    }
}

void vm_hw_glfw_resize_window_callback(GLFWwindow window, int w, int h)
{
    void* ud;
    hw_t * device;
    uint32_t device_id;
    ud = glfwGetWindowUserPointer(window);
    device = (hw_t*) ud;
    device_id = device->id;
    if (device_id == LEM1802_ID)
    {
        vm_hw_lem1802_glfw_resize_handler(window, w, h);
    }
    else if (device_id == SPED3_ID)
    {
        // no resize handler is implemented for the SPED3 yet
    }
}
