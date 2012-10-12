/**

    File:   hwsped3.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    Jose Manuel Diez
        David Herberth

    Description: Implements the SPED-3 specification.
**/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <debug.h>
#include <math.h>

#include "hw.h"
#include "hwsped3.h"
#include "dcpu.h"
#include "dcpubase.h"
#include "dcpuhook.h"
#include "dcpuops.h"

#define SPED3_SPEED 0.8f

// Credit for the rotation logic: https://github.com/SirCmpwn/Tomato/blob/master/Tomato/Hardware/SPED3.cs#L34

float compare_degrees(float A, float B)
{
    if(A > 180 && B < 180)
        return (360 - A) + B;
    if(A < 180 && B > 180)
        return -((360 - B) + A);

    return B - A;
}

void vm_hw_sped3_update_rot(struct sped3_hardware* hw)
{
    if (hw->rot_current != hw->rot_target)
    {
        if(abs(hw->rot_target - hw->rot_current) < SPED3_SPEED)
        {
            hw->rot_current = hw->rot_target;
        }
        else
        {
            if(compare_degrees(hw->rot_current, hw->rot_target) <= 0)
                hw->rot_current -= SPED3_SPEED;
            else
                hw->rot_current += SPED3_SPEED;
            
            while(hw->rot_current < 0)
                hw->rot_current += 360.f;
          
            hw->rot_current = fmod(hw->rot_current, 360);
        }
    }
    else hw->state = SPED3_STATE_RUNNING;
}

void vm_hw_sped3_set_color(struct sped3_hardware* hw, uint8_t cc, uint8_t intensity)
{
    float k = 1.f;

    if (intensity == 1)
        k = 0.5f;

    switch (cc)
    {
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

void vm_hw_sped3_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct sped3_hardware* hw = (struct sped3_hardware*)ud;
    int i;
    uint16_t firstword, secondword;
    uint8_t cc, intensity;
    float x, y, z;

    glfwMakeContextCurrent(hw->window);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.f, -4.f, 0.f,
          0.f, 0.f, 0.f,
          0.f, 0.f, 1.f);

    vm_hw_sped3_update_rot(hw);

    glRotatef(hw->rot_current, 0.f, 0.f, 1.f);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINE_STRIP);
    for (i = 0; i < MIN(hw->num, 128); i++)
    {
        firstword  = vm->ram[hw->mem + i * 2];
        secondword = vm->ram[hw->mem + (i * 2) + 1];

        x = (float)(firstword & 0xff) / 256 * 2 - 1;
        y = (float)(firstword >> 8) / 256 * 2 - 1;
        z = (float)(secondword & 0xff) / 256 * 2 - 1;

        cc = (secondword >> 8) & 0x3;
        intensity = secondword >> 11;

        glColor3f(1.f, 1.f, 1.f);
        vm_hw_sped3_set_color(hw, cc, intensity);
        glVertex3f(x, y, z);
    }
    glEnd();

    glfwSwapBuffers(hw->window);

    glfwPollEvents();
}

void vm_hw_sped3_interrupt(vm_t* vm, void* ud)
{
    struct sped3_hardware* hw = (struct sped3_hardware*)ud;

    switch (vm->registers[REG_A])
    {
        case SPED3_INTERRUPT_POLL:
            vm->registers[REG_B] = hw->state;
            break;
        case SPED3_INTERRUPT_MAP:
            hw->mem = vm->registers[REG_X];
            hw->num = vm->registers[REG_Y];
            hw->state = SPED3_STATE_RUNNING;
            break;
        case SPED3_INTERRUPT_ROTATE:
            hw->rot_target = vm->registers[REG_X];
            hw->state = SPED3_STATE_TURNING;
            break;
    }
}

int vm_hw_sped3_close(GLFWwindow w)
{
    void* ud = glfwGetWindowUserPointer(w);

    vm_hw_sped3_free(ud);
    return 0;
}

void vm_hw_sped3_init(vm_t* vm)
{
    struct sped3_hardware* hw;

    hw = malloc(sizeof(struct sped3_hardware));
    hw->cycle_hook = 0;
    hw->width = 400;
    hw->height = 400;
    hw->mem = 0;
    hw->num = 0;
    hw->rot_target = 0;
    hw->rot_current = 0;
    hw->state = SPED3_STATE_NO_DATA;
    hw->vm = vm;

    hw->device.id = 0x42babf3c;
    hw->device.version = 0x0003;
    hw->device.manufacturer = 0x1eb37e91;
    hw->device.handler = &vm_hw_sped3_interrupt;
    hw->device.userdata = hw;

    hw->cycle_hook = vm_hook_register(vm, &vm_hw_sped3_cycle, HOOK_ON_60HZ, hw);
    hw->hw_id = vm_hw_register(vm, hw->device);


    // TODO: check for errors and cry about it
    //glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    hw->window = (GLFWwindow) glfwCreateWindow(hw->width, hw->height, GLFW_WINDOWED, "SPED-3", NULL);

    glfwSetWindowUserPointer(hw->window, hw);
    glfwMakeContextCurrent(hw->window);
    glfwSwapInterval(0);
    
    glfwSetTime(0.0);
    glfwSetWindowCloseCallback(&vm_hw_sped3_close);

    glViewport(0, 0, hw->width, hw->height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.f, (GLfloat) hw->width / (GLfloat) hw->height, 1.f, 1000.f);
}

void vm_hw_sped3_free(void* ud)
{
    struct sped3_hardware* hw = (struct sped3_hardware*)ud;

    glfwDestroyWindow(hw->window);
    vm_hook_unregister(hw->vm, hw->cycle_hook);
    vm_hw_unregister(hw->vm, hw->hw_id);
    free(hw);
}
