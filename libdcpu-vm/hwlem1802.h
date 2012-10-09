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

struct lem1802_hardware
{
	hw_t device;

	uint16_t cycle_hook;
	uint16_t write_hook;
	uint16_t break_hook;
	uint16_t hw_id;
	
	uint16_t palette_location;
	uint16_t font_location;
	uint16_t screen_location;
	
	uint16_t blink_status;
	unsigned int blink_tick;
	unsigned char * glfw_texture;
	GLint texture_id;
	
	
	

	GLFWwindow window;
	int width, height;


	vm_t* vm;
};


void vm_hw_lem1802_init(vm_t* vm);
void vm_hw_lem1802_write(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_lem1802_free(void* ud);
void vm_hw_lem1802_interrupt(vm_t* vm, void* ud);

// TODO relocate the functions somewhere else:
void vm_hw_lem1802_glfw_resize_handler(GLFWwindow window, int w, int h);
void vm_hw_lem1802_init_glfw(struct lem1802_hardware* hw);
void vm_hw_lem1802_glfw_draw(struct lem1802_hardware* hw);
void vm_hw_lem1802_update_texture(struct lem1802_hardware* hw);

#endif
