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
#include "hwsped3.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

uint16_t sped3_hook_id = 0;
uint16_t sped3_hw_id = 0;

GLFWwindow sped3_window;
int sped3_width = 400, sped3_height = 400;
double last_redraw = 0;

uint16_t sped3_mem = 0;
uint16_t sped3_num = 0;
uint16_t sped3_rot_target = 0;
float sped3_rot_current = 0;

vm_t* g_vm;

uint16_t vm_hw_sped3_state(void) {
    return SPED3_STATE_RUNNING;
}

void vm_hw_sped3_update_rot() {
    if(sped3_rot_current != sped3_rot_target) {
	sped3_rot_current += 0.8f;
    }
}

void vm_hw_sped3_set_color(uint8_t cc, uint8_t intensity) {
    float k = 1.f;

    if(intensity == 1)
	k = 0.5f;
	
    switch(cc) {
	case 0: 
	    glColor3f(0.05f * k, 0.05f * k, 0.05f * k);
	    break;
	case 1:
	    glColor3f(1.f * k, 0.f, 0.f);
	    break;
	case 2:
	    glColor3f(0.f, 1.f * k, 0.f);
	    break;
	case 3:
	    glColor3f(0.f, 0.f, 1.f * k);
	    break;
    }
}




void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud) {
    int i;
    uint16_t firstword, secondword;
    uint8_t cc, intensity;
    float x, y, z;

    glfwMakeContextCurrent(sped3_window);

    if((glfwGetTime() - last_redraw > 0.1)) {
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	last_redraw = glfwGetTime();

	vm_hw_sped3_update_rot();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(  0.f, -4.f, 0.f,
		    0.f, 0.f, 0.f,
		    0.f, 0.f, 1.f);

	glRotatef(sped3_rot_current, 0.f, 0.f, 1.f);
	glTranslatef(0.f, 0.f, 2.f);

	glBegin(GL_LINE_STRIP);
	for(i = 0; i < sped3_num; i++) {
	    firstword  = vm->ram[sped3_mem + i * 2];
	    secondword = vm->ram[sped3_mem + (i * 2) + 1];

	    // Translate SPED-3 coordinates into OpenGL coordinates. 
	    x = (float) (firstword & 0xff) / 256 * 2 - 1;
	    // y+ is into the world in SPED-3, z- is into the screen in OpenGL
	    z = -(float) (firstword >> 8) / 256 * 2 - 1; 
	    // z+ is up in SPED-3, y+ is up in OpenGL
	    y = (float) (secondword & 0xff) / 256 * 2 - 1;
    
	    cc = (secondword >> 8) & 0x3;
	    intensity = secondword >> 11; 
	 
	    glColor3f(1.f, 1.f, 1.f);
	    vm_hw_sped3_set_color(cc, intensity);
	    glVertex3f(x, y, z);
	}
	glEnd();

	glfwSwapBuffers(sped3_window);
    }
    
    glfwPollEvents();
}

void vm_hw_sped3_interrupt(vm_t* vm, void* ud) {
    switch(vm->registers[REG_A]) {
	case SPED3_INTERRUPT_POLL:
	    vm->registers[REG_B] = vm_hw_sped3_state();
	    break;
	case SPED3_INTERRUPT_MAP:
	    sped3_mem = vm->registers[REG_X];
	    sped3_num = vm->registers[REG_Y];
	    break;
	case SPED3_INTERRUPT_ROTATE:
	    sped3_rot_target = vm->registers[REG_X] % 360;
	    break;
   }
}

int vm_hw_sped3_close(GLFWwindow w) {
    vm_hw_sped3_free(g_vm);
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
    //glfwWindowHint(GLFW_DEPTH_BITS, 16);
    sped3_window = (GLFWwindow) glfwCreateWindow(sped3_width, sped3_height, GLFW_WINDOWED, "SPED-3", NULL);
    
    glfwMakeContextCurrent(sped3_window);
    glfwSwapInterval(1);
    
    glfwSetTime(0.0);	 
    glfwSetWindowCloseCallback(&vm_hw_sped3_close);
    
    glViewport(0, 0, sped3_width, sped3_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.f, (GLfloat) sped3_width / (GLfloat) sped3_height, 1.f, 1000.f);

    g_vm = vm;
}

void vm_hw_sped3_free(vm_t* vm)
{
	glfwDestroyWindow(sped3_window);
    vm_hook_unregister(vm, sped3_hook_id);
	vm_hw_unregister(vm, sped3_hw_id);
}
