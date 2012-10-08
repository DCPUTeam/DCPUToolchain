/**

	File:		lem1802.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes
			José manuel Díez

	Description:	Handles LEM1802 functions

**/

#ifndef __DCPU_HW_LEM1802_H
#define __DCPU_HW_LEM1802_H

#include <GL/glfw3.h>
#include "dcpu.h"

#define LEM1802_MEM_MAP_SCREEN		0
#define LEM1802_MEM_MAP_FONT		1
#define LEM1802_MEM_MAP_PALETTE		2
#define LEM1802_SET_BORDER_COLOR	3
#define LEM1802_MEM_DUMP_FONT		4
#define LEM1802_MEM_DUMP_PALETTE	5

void vm_hw_lem1802_init(vm_t* vm);
void vm_hw_lem1802_write(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_lem1802_free(vm_t* vm);
void vm_hw_lem1802_interrupt(vm_t* vm, void* ud);

// TODO relocate the functions somewhere else:
void vm_hw_lem1802_glfw_resize_handler(GLFWwindow window, int w, int h);
void vm_hw_lem1802_init_glfw();
void vm_hw_lem1802_glfw_draw();
void vm_hw_lem1802_update_texture(vm_t* vm);

#endif
