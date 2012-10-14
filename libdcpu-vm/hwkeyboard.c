/**

    File:	hwkeyboard.c

    Project:	DCPU-16 Tools
    Component:	LibDCPU-vm

    Authors:	Jose Manuel Diez

    Description: Implements the keyboard specification.
**/

#include <GL/glfw3.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <debug.h>
#include "hw.h"
#include "hwkeyboard.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

// We can't set a window pointer, so a global variable will have to do here. Ugly...
struct keyboard_hardware* g_hw;

void vm_hw_keyboard_append_to_buffer(struct keyboard_hardware* hw, int index)
{
	hw->buffer[hw->buffer_idx_w++] = index;
	if(hw->buffer_idx_w == KB_BUFFER_COUNT)
	{
		hw->buffer_idx_w = 0;
		hw->buffer_idx_r = 0;
	}
}


void vm_hw_keyboard_handle_char(GLFWwindow w, int c)
{
	if(c > 0x20 || c < 0x7f)
		vm_hw_keyboard_append_to_buffer(g_hw, c);
}

void vm_hw_keyboard_handle_key(GLFWwindow w, int key, int state)
{
	uint16_t keycode = 0;
	uint16_t index = 0;

	switch(key)
	{
		case GLFW_KEY_BACKSPACE:
			g_hw->pressed[0x10] = state;
			index = 0x10;
			break;
		case GLFW_KEY_ENTER:
			g_hw->pressed[0x11] = state;
			index = 0x11;
			break;
		case GLFW_KEY_INSERT:
			g_hw->pressed[0x12] = state;
			index = 0x12;
			break;
		case GLFW_KEY_DELETE:
			g_hw->pressed[0x13] = state;
			index = 0x13;
			break;

		case GLFW_KEY_UP:
			g_hw->pressed[0x80] = state;
			index = 0x80;
			break;
		case GLFW_KEY_DOWN:
			g_hw->pressed[0x81] = state;
			index = 0x81;
			break;
		case GLFW_KEY_LEFT:
			g_hw->pressed[0x82] = state;
			index = 0x82;
			break;
		case GLFW_KEY_RIGHT:
			g_hw->pressed[0x83] = state;
			index = 0x83;
			break;

		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			g_hw->pressed[0x91] = state;
			index = 0x91;
			break;
			 
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			g_hw->pressed[0x90] = state;
			g_hw->lowercase = !state;
			index = 0x90;
			break;

        case GLFW_KEY_F8:
            if(state)
            {
                g_hw->vm->radiation = !g_hw->vm->radiation;
                printf("DCPU radiation: %s\n", g_hw->vm->radiation ? "ACTIVE" : "INACTIVE");
            }
            break;
		
		default:
			if(g_hw->lowercase)
			{
				keycode = tolower(key);
			}
			else
			{
				keycode = key;
			}

			g_hw->pressed[keycode] = state;
			index = keycode;
			break;
	}

	// Char events are handled by the char handler.
	if(state == true && (index < 0x20 || index > 0x7f))
	{
		vm_hw_keyboard_append_to_buffer(g_hw, index);
	}

	if(g_hw->interrupt_message)
	{
		vm_interrupt(g_hw->vm, g_hw->interrupt_message);
	}
}

void vm_hw_keyboard_interrupt(vm_t* vm, void* ud)
{
	struct keyboard_hardware* hw = (struct keyboard_hardware*) ud;
	int i = 0;

	switch(vm->registers[REG_A])
	{
		case KB_INTERRUPT_CLEAR:
			for(i = 0; i < KB_BUFFER_COUNT; i++)
			{
				hw->buffer[i] = 0;
			}

			break;
		case KB_INTERRUPT_STORE:
			if(hw->buffer_idx_w > 0)
			{
				hw->buffer_idx_r++;

				if(hw->buffer_idx_r <= KB_BUFFER_COUNT)
				{
					vm->registers[REG_C] = hw->buffer[hw->buffer_idx_r - 1];
				}

				hw->buffer_idx_w--;
				if(hw->buffer_idx_w == 0)
				{
					hw->buffer_idx_r = 0;
				}
			}
			else
			{
				vm->registers[REG_C] = 0;
			}

			break;
		case KB_INTERRUPT_TEST:
			if(vm->registers[REG_B] > 0 && vm->registers[REG_B] <= KB_PRESSED_COUNT)
			{
				vm->registers[REG_C] = hw->pressed[vm->registers[REG_B]];
			}

			break;
		case KB_INTERRUPT_MSG:
			hw->interrupt_message = vm->registers[REG_B];
			break;
	}
}

void vm_hw_keyboard_init(vm_t* vm)
{
	struct keyboard_hardware* hw;

	hw = malloc(sizeof(struct keyboard_hardware));
	hw->buffer = malloc(sizeof(uint16_t) * KB_BUFFER_COUNT);
	hw->buffer_idx_w = 0;
	hw->buffer_idx_r = 0;
	hw->pressed = malloc(sizeof(bool) * KB_PRESSED_COUNT);
	hw->interrupt_message = 0;
	hw->lowercase = true;
	hw->vm = vm;

	hw->device.id = 0x30cf7406;
	hw->device.version = 0x0001;
	hw->device.manufacturer = 0x0;
	hw->device.handler = &vm_hw_keyboard_interrupt;
	hw->device.userdata = hw;

	hw->hw_id = vm_hw_register(vm, hw->device);

	g_hw = hw;
}


