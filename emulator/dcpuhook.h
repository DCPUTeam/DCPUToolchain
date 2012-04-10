/**

	File:			dcpuscrn.h

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Declares the hooking mechanism for observing
					when the VM's RAM changes.

**/

#ifndef __DCPUHOOK_H
#define __DCPUHOOK_H

#include "dcpu.h"

#ifdef PRIVATE_VM_ACCESS
void vm_hook_fire(vm_t* vm, uint16_t pos);
#endif

typedef void (*vm_hook)(vm_t*, uint16_t pos);

// Pass 0 to mode as it is currently unused and hooks only fire on VM RAM write
uint16_t vm_hook_register(vm_t* vm, vm_hook hook, uint16_t mode);
void vm_hook_unregister(vm_t* vm, uint16_t id, uint16_t mode);

#endif