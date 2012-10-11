/**

    File:	hwm35fd.c

    Project:	DCPU-16 Tools
    Component:	LibDCPU-vm

    Authors:	Jose Manuel Diez

    Description: Implements the M35FD specification.
**/

#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include <iio.h>
#include "hw.h"
#include "hwm35fd.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

void vm_hw_m35fd_set_state(struct m35fd_hardware* hw, uint16_t state)
{
	hw->state = state;
	if(hw->interrupt_msg)
	{
		vm_interrupt(hw->vm, hw->interrupt_msg);
	}
}

void vm_hw_m35fd_set_error(struct m35fd_hardware* hw, uint16_t error)
{
	hw->last_error = error;
	if(hw->interrupt_msg)
	{
		vm_interrupt(hw->vm, hw->interrupt_msg);
	}
}

void vm_hw_m35fd_interrupt(vm_t* vm, void* ud)
{
	struct m35fd_hardware* hw = (struct m35fd_hardware*)ud;

	switch (vm->registers[REG_A])
	{
		case M35FD_INTERRUPT_POLL:
			vm->registers[REG_B] = hw->state;
			vm->registers[REG_C] = hw->last_error;

			hw->last_error = M35FD_ERROR_NONE;
		case M35FD_INTERRUPT_MSG:
			hw->interrupt_msg = vm->registers[REG_X];
			break;
		case M35FD_INTERRUPT_READ:
			if((hw->state == M35FD_STATE_READY || hw->state == M35FD_STATE_READY_WP) 
				&& hw->reading == false && hw->writing == false)
			{
				hw->reading = true;
				hw->sector = vm->registers[REG_X];
				hw->position = vm->registers[REG_Y];

				vm_hw_m35fd_set_state(hw, M35FD_STATE_READING);
				vm->registers[REG_B] = 1;
			}
			else
			{
				vm->registers[REG_B] = 0;
				vm_hw_m35fd_set_error(hw, M35FD_ERROR_BUSY);
			}
			break;
		case M35FD_INTERRUPT_WRITE:
			if((hw->state == M35FD_STATE_READY) 
				&& hw->reading == false && hw->writing == false)
			{
				hw->writing = true;
				hw->sector = vm->registers[REG_X];
				hw->position = vm->registers[REG_Y];

				vm_hw_m35fd_set_state(hw, M35FD_STATE_WRITING);
				vm->registers[REG_B] = 1;
			}
			else
			{
				vm->registers[REG_B] = 0;
				vm_hw_m35fd_set_error(hw, M35FD_ERROR_BUSY);
			}
			break;
	}
}

void vm_hw_m35fd_reset_state(struct m35fd_hardware* hw) 
{
	hw->sector = 0;
	hw->reading = false;
	hw->writing = false;
	hw->position = 0;
	vm_hw_m35fd_set_state(hw, (hw->write_protected) ? M35FD_STATE_READY_WP : M35FD_STATE_READY);
}	

void vm_hw_m35fd_cycle(vm_t* vm, uint16_t pos, void* ud) 
{
	struct m35fd_hardware* hw = (struct m35fd_hardware*)ud;
	int i = 0;
	uint16_t word = 0;

	if(hw->state == M35FD_STATE_NO_MEDIA && (hw->reading || hw->writing))
	{
		vm_hw_m35fd_set_error(hw, M35FD_ERROR_NO_MEDIA);
		vm_hw_m35fd_reset_state(hw);
		return;
	}

	if(hw->disk == NULL)
	{
		vm_hw_m35fd_set_error(hw, M35FD_ERROR_EJECT);
		vm_hw_m35fd_reset_state(hw);
	}

	if(hw->sector > 1440)
	{
		// There is no error for "The requested sector does not exist."

		vm_hw_m35fd_reset_state(hw);
	}

	if(hw->reading) 
	{
		fseek(hw->disk, hw->sector * 512, SEEK_SET);
		for(i = 0; i < 512; i++) {
			iread(&word, hw->disk);
			vm->ram[hw->position + i] = word;

		}
		vm_hw_m35fd_reset_state(hw);
	}

	if(hw->writing)
	{
		printf("write: here we go\n");
		fseek(hw->disk, hw->sector * 512, SEEK_SET);
		for(i = 0; i < 512; i++) {
			word = vm->ram[hw->position + i];
			iwrite(&word, hw->disk);
			fflush(hw->disk);
		}
		vm_hw_m35fd_reset_state(hw);
	}
}

void vm_hw_m35fd_load_disk(struct m35fd_hardware* hw, const char* path)
{
	hw->disk = fopen(path, "rb+");
	if(hw->disk != NULL)
	{
		vm_hw_m35fd_set_state(hw, (hw->write_protected) ? M35FD_STATE_READY_WP : M35FD_STATE_READY);
	}
}

void vm_hw_m35fd_init(vm_t* vm)
{
	struct m35fd_hardware* hw;

	hw = malloc(sizeof(struct m35fd_hardware));
	hw->hw_hook = 0;
	hw->hw_id = 0;
	hw->interrupt_msg = 0;

	hw->state = M35FD_STATE_NO_MEDIA;
	hw->last_error = M35FD_ERROR_NONE;

	hw->disk = NULL;
	hw->sector = 0;
	hw->position = 0;

	hw->reading = false;
	hw->writing = false;
	hw->write_protected = false;

	hw->vm = vm;

	hw->device.id = 0x12345678;
	hw->device.version = 0x000b;
	hw->device.manufacturer = 0x1eb37e91;
	hw->device.handler = &vm_hw_m35fd_interrupt;
	hw->device.userdata = hw;

	hw->hw_hook = vm_hook_register(vm, &vm_hw_m35fd_cycle, HOOK_ON_60HZ, hw);
	hw->hw_id = vm_hw_register(vm, hw->device);

	vm_hw_m35fd_load_disk(hw, "m35fd.bin");
}


