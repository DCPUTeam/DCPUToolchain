/**

    File:       GLFWutils.h

    Project:        DCPU-16 Toolchain
    Component:      LibDCPU-VM

    Authors:        Patrick Flick

    Description:    Declares utility functions for glfw.

**/
#include <GL/glfw3.h>

int vm_hw_glfw_close_window_callback(GLFWwindow w);
void vm_hw_glfw_resize_window_callback(GLFWwindow window, int w, int h);
