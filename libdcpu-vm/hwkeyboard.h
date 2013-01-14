/**

    File:       hwkeyboard.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    Jose Manuel Diez

    Description: Function signatures for the keyboard.
**/

#ifndef __DCPUKEYBOARD_H
#define __DCPUKEYBOARD_H

#include <GL/glfw3.h>
#include "vm.h"

#define KB_BUFFER_COUNT    100
#define KB_PRESSED_COUNT   0x92

#define KB_INTERRUPT_CLEAR 0
#define KB_INTERRUPT_STORE 1
#define KB_INTERRUPT_TEST  2
#define KB_INTERRUPT_MSG   3

#define KB_KEY_BACKSPACE 0x10
#define KB_KEY_ENTER     0x11
#define KB_KEY_INSERT    0x12
#define KB_KEY_DELETE    0x13
#define KB_KEY_UP        0x80
#define KB_KEY_DOWN      0x81
#define KB_KEY_LEFT      0x82
#define KB_KEY_RIGHT     0x83
#define KB_KEY_SHIFT     0x90
#define KB_KEY_CONTROL   0x91

struct keyboard_hardware
{
    hw_t device;
    vm_t* vm;

    uint16_t hw_id;

    uint16_t* buffer;
    uint16_t buffer_idx_r;
    uint16_t buffer_idx_w;

    bool* pressed;
    // Left and right modifiers need to be tracked seperately
    bool control_left;
    bool control_right;
    bool shift_left;
    bool shift_right;


    uint16_t interrupt_message;
    bool lowercase;
};

void vm_hw_keyboard_handle_char(GLFWwindow w, int key);
void vm_hw_keyboard_handle_key(GLFWwindow w, int key, int state);

void vm_hw_keyboard_interrupt(vm_t* vm, void* ud);
void vm_hw_keyboard_init(vm_t* vm);
void vm_hw_keyboard_free(void* ud);

#endif
