/**

	File:		hwsped3.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:    Jose Manuel Diez
	
	Description: Function signatures and sped-3 modes.	
**/

#ifndef __DCPUHWSPED3_H
#define __DCPUHWSPED3_H 

#include "dcpu.h"

#define SPED3_STATE_NO_DATA 0
#define SPED3_STATE_RUNNING 1
#define SPED3_STATE_TURNING 2

#define SPED3_ERROR_NONE    0
#define SPED3_ERROR_BROKEN  0xffff

void vm_hw_sped3_init(vm_t* vm);
void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud);
void vm_hw_sped3_free(vm_t* vm);

#endif
