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

GLFWwindow sped3_window;
int sped3_width = 800, sped3_height = 600;

uint16_t sped3_mem = 0;

void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud) {
    printf("sped3 cycle\n");
    glfwMakeContextCurrent(sped3_window);
    glfwGetWindowSize(sped3_window, &sped3_width, &sped3_height);
    sped3_height = sped3_height > 0 ? sped3_height : 1;

    glViewport(0, 0, sped3_width, sped3_height);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.f, (GLfloat) sped3_width / (GLfloat) sped3_height, 1.f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(	 0.f, 1.f, 0.f,
		0.f, 20.f, 0.f,
		0.f, 0.f, 1.f);

    glTranslatef(0.f, 14.f, 0.f);
    glRotatef(0.3f * 100.f, 0.f, 0.f, 1.f);

    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-5.f, 0.f, -4.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(5.f, 0.f, -4.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 6.f);
    glEnd();

    glfwSwapBuffers(sped3_window);
    
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
    sped3_window = (GLFWwindow) glfwCreateWindow(sped3_width, sped3_height, GLFW_WINDOWED, "SPED-3", NULL);
    
    glfwMakeContextCurrent(sped3_window);
    glfwSwapInterval(1);
    
    glfwSetTime(0.0);	 
}

void vm_hw_sped3_free(vm_t* vm)
{
	vm_hook_unregister(vm, sped3_hook_id);
	vm_hw_unregister(vm, sped3_hw_id);
}
