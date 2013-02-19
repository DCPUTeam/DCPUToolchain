/**

    File:   hwkeyboard.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    Jose Manuel Diez

    Description: Implements the keyboard specification.
**/

#include <GL/glfw3.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <debug.h>
#include "vm.h"
#include "dcpuhook.h"
#include "dcpuops.h"
#include "hw.h"
#include "hwkeyboard.h"

// We can't set a window pointer, so a global variable will have to do here. Ugly...
struct keyboard_hardware* g_hw;

void vm_hw_keyboard_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct keyboard_hardware* hw = (struct keyboard_hardware*) ud;
    
    if (hw->pending_interrupt)
    {
        if (hw->interrupt_message)
        {
            vm_interrupt(hw->vm, hw->interrupt_message);
        }
        hw->pending_interrupt = false;
    }
}

void vm_hw_keyboard_append_to_buffer(struct keyboard_hardware* hw, int index)
{
    // Sanity checking
    if (index >= 0)
    {
        hw->buffer[hw->buffer_idx_w++] = (uint16_t) index;
        // Should the write index wrap back to the beginning?
        if (hw->buffer_idx_w >= KB_BUFFER_COUNT)
        {
            hw->buffer_idx_w = 0;
        }

        // Has the write index caught up with the read index?
        if (hw->buffer_idx_r == hw->buffer_idx_w)
        {
            // The oldest character has been discarded.
            // Advance the read index to compensate.
            hw->buffer_idx_r++;

            // Should the read index wrap back to the beginning?
            if (hw->buffer_idx_r >= KB_BUFFER_COUNT)
            {
                hw->buffer_idx_r = 0;
            }
        }
    }
}


void vm_hw_keyboard_handle_char(GLFWwindow w, int c)
{
    // Sanity checking
    if ( 0 <= c && c < KB_PRESSED_COUNT)
        vm_hw_keyboard_append_to_buffer(g_hw, c);
}

void vm_hw_keyboard_handle_key(GLFWwindow w, int key, int state)
{
    bool key_state = (bool) state;
    int keycode = 0;
    int index = 0;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE:
            g_hw->pressed[KB_KEY_BACKSPACE] = key_state;
            index = KB_KEY_BACKSPACE;
            break;
        case GLFW_KEY_KP_ENTER:    /* numpad enter */
        case GLFW_KEY_ENTER:
            g_hw->pressed[KB_KEY_ENTER] = key_state;
            index = KB_KEY_ENTER;
            break;
        case GLFW_KEY_INSERT:
            g_hw->pressed[KB_KEY_INSERT] = key_state;
            index = KB_KEY_INSERT;
            break;
        case GLFW_KEY_DELETE:
            g_hw->pressed[KB_KEY_DELETE] = key_state;
            index = KB_KEY_DELETE;
            break;

        case GLFW_KEY_UP:
            g_hw->pressed[KB_KEY_UP] = key_state;
            index = KB_KEY_UP;
            break;
        case GLFW_KEY_DOWN:
            g_hw->pressed[KB_KEY_DOWN] = key_state;
            index = KB_KEY_DOWN;
            break;
        case GLFW_KEY_LEFT:
            g_hw->pressed[KB_KEY_LEFT] = key_state;
            index = KB_KEY_LEFT;
            break;
        case GLFW_KEY_RIGHT:
            g_hw->pressed[KB_KEY_RIGHT] = key_state;
            index = KB_KEY_RIGHT;
            break;

        case GLFW_KEY_LEFT_CONTROL:
            g_hw->control_left = key_state;
            g_hw->pressed[KB_KEY_CONTROL] = g_hw->control_left || g_hw->control_right;
            index = KB_KEY_CONTROL;
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            g_hw->control_right = key_state;
            g_hw->pressed[KB_KEY_CONTROL] = g_hw->control_left || g_hw->control_right;
            index = KB_KEY_CONTROL;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            g_hw->shift_left = key_state;
            g_hw->pressed[KB_KEY_SHIFT] = g_hw->shift_left || g_hw->shift_right;
            index = KB_KEY_SHIFT;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            g_hw->shift_right = key_state;
            g_hw->pressed[KB_KEY_SHIFT] = g_hw->shift_left || g_hw->shift_right;
            index = KB_KEY_SHIFT;
            break;

        case GLFW_KEY_F8:
            if (key_state)
            {
                g_hw->vm->radiation = !g_hw->vm->radiation;
                printf("DCPU radiation: %s\n", g_hw->vm->radiation ? "ACTIVE" : "INACTIVE");
            }
            break;

        default:
            // Letter keys use the uppercase values
            index = key;
            if(index < KB_PRESSED_COUNT)
                g_hw->pressed[index] = key_state;

            // Have the lowercase letters mirror the state
            // to make the behavior more intuitive.
            keycode = tolower(key);
            if(keycode < KB_PRESSED_COUNT)
                g_hw->pressed[keycode] = key_state;

            break;
    }

    // Char events are handled by the char handler.
    // No index should be larger than 0x91: Control
    if (key_state == true && (index < 0x20 || index > 0x7f) && index < KB_PRESSED_COUNT)
    {
        vm_hw_keyboard_append_to_buffer(g_hw, index);
    }

    if (g_hw->interrupt_message)
    {
        g_hw->pending_interrupt = true;
    }
}

void vm_hw_keyboard_interrupt(vm_t* vm, void* ud)
{
    struct keyboard_hardware* hw = (struct keyboard_hardware*) ud;
    int i = 0;

    switch (vm->registers[REG_A])
    {
        case KB_INTERRUPT_CLEAR:
            for (i = 0; i < KB_BUFFER_COUNT; i++)
            {
                hw->buffer[i] = 0;
            }

            break;
        case KB_INTERRUPT_STORE:
            if (hw->buffer_idx_r == hw->buffer_idx_w)
            {
                // Do nothing when read idx caught up with write idx
                vm->registers[REG_C] = 0;
            }
            else
            {
                vm->registers[REG_C] = hw->buffer[hw->buffer_idx_r];

                hw->buffer_idx_r++;
                // Should the read index wrap back to the beginning?
                if (hw->buffer_idx_r >= KB_BUFFER_COUNT)
                {
                    hw->buffer_idx_r = 0;
                }
                
            }
                
            break;
        case KB_INTERRUPT_TEST:
            if (vm->registers[REG_B] > 0 && vm->registers[REG_B] <= KB_PRESSED_COUNT)
                vm->registers[REG_C] = hw->pressed[vm->registers[REG_B]];
            else
                vm->registers[REG_C] = 0;

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
    // Initialize all keys to released
    memset(hw->pressed, (bool) GLFW_RELEASE, sizeof(bool) * KB_PRESSED_COUNT);

    hw->control_left = (bool) GLFW_RELEASE;
    hw->control_right = (bool) GLFW_RELEASE;
    hw->shift_left = (bool) GLFW_RELEASE;
    hw->shift_right = (bool) GLFW_RELEASE;
    
    hw->interrupt_message = 0;
    hw->lowercase = true;
    hw->vm = vm;
    hw->cycle_id = vm_hook_register(vm, &vm_hw_keyboard_cycle, HOOK_ON_POST_CYCLE, hw);

    hw->device.id = 0x30cf7406;
    hw->device.version = 0x0001;
    hw->device.manufacturer = 0x0;
    hw->device.handler = &vm_hw_keyboard_interrupt;
    hw->device.free_handler = NULL;
    hw->device.userdata = hw;

    hw->hw_id = vm_hw_register(vm, &hw->device);

    g_hw = hw;
}


