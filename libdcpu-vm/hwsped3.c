/**

	File:		hwsped3.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:    Jose Manuel Diez	

	Description: Implements the SPED-3 specification.	
**/

#include <GL/glfw3.h>

#include <stdio.h>
#include <time.h>
#include <debug.h>
#include "hw.h"
#include "hwtimer.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

uint16_t sped3_hook_id = 0;
uint16_t sped3_hw_id = 0;

GLFWwindow window;
int sped3_width = 800, sped3_height = 600;

void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud) {
    // opengl here
}

void vm_hw_sped3_interrupt(vm_t* vm, void* ud) {
    // handle interrupts
    printf("SPED3 interrupted!\n");
}

void vm_hw_sped3_init(vm_t* vm)
{
	hw_t sped3;

	sped3.id = 0x42babf3c;
    sped3.version = 0x0003;
	sped3.manufacturer = 0x1eb37e91;
	sped3.handler = &vm_hw_sped3_interrupt;

	sped3_hook_id = vm_hook_register(vm, &vm_hw_sped3_cycle, HOOK_ON_POST_CYCLE, NULL);
	sped3_hw_id = vm_hw_register(vm, sped3);


    // TODO: check for errors and cry about it
    glfwInit(); 
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    window = (GLFWwindow) glfwCreateWindow(sped3_width, sped3_height, GLFW_WINDOWED, "SPED-3", NULL);

}

void vm_hw_sped3_free(vm_t* vm)
{
	vm_hook_unregister(vm, sped3_hook_id);
	vm_hw_unregister(vm, sped3_hw_id);
}
