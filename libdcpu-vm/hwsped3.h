/**

	File:		hwsped3.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	Jose Manuel Diez
			David Herberth
	
	Description: Function signatures and sped-3 modes.	
**/

#ifndef __DCPUHWSPED3_H
#define __DCPUHWSPED3_H 

#include "dcpu.h"

#define SPED3_INTERRUPT_POLL   0
#define SPED3_INTERRUPT_MAP    1
#define SPED3_INTERRUPT_ROTATE 2

#define SPED3_STATE_NO_DATA 0
#define SPED3_STATE_RUNNING 1
#define SPED3_STATE_TURNING 2

#define SPED3_ERROR_NONE    0
#define SPED3_ERROR_BROKEN  0xffff

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

struct sped3_hardware
{
	hw_t device;
	
	uint16_t cycle_hook;
	uint16_t hw_id;

	GLFWwindow window;
	int width, height;

	uint16_t mem;
	uint16_t num;
	uint16_t rot_target;
	float rot_current;

	uint16_t state;
	vm_t* vm;
};

void vm_hw_sped3_init(vm_t* vm);
void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_sped3_free(void* ud);

#endif
