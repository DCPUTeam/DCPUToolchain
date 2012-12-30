/**

    File:       lem1802.c

    Project:        DCPU-16 Tools
    Component:      LibDCPU-vm

    Authors:        James Rhodes
            José Manuel Díez
            Tyrel Haveman
            Patrick Flick

    Description:    Handles LEM1802 functions

**/

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <string.h>
#include <bstring.h>
#include <debug.h>
#include <GL/glfw3.h>
#include <stdlib.h>
#include "glfwutils.h"

#include "vm.h"
#include "dcpuhook.h"
#include "hw.h"
#include "hwkeyboard.h"
#include "hwlem1802.h"
#include "hwlem1802mem.h"
#include "hwlem1802util.h"
#include "hwioascii.h"

GLint next_texture_id = 13;

void vm_hw_lem1802_write(vm_t* vm, uint16_t pos, void* ud)
{
    uint16_t location_screen, location_font, location_palette;
    struct lem1802_hardware* hw = (struct lem1802_hardware*)ud;

    location_screen = vm_hw_lem1802_mem_get_screen(hw);
    location_font = vm_hw_lem1802_mem_get_font(hw);
    location_palette = hw->palette_location;

    // Are we updating a general cell?
    if (location_screen != HW_LEM1802_DISCONNECTED && pos >= location_screen && pos <= location_screen + 0x17F)
    {
        hw->screen_was_updated = 1;
    }
    // Are we updating a font character?
    else if (location_font != HW_LEM1802_DEFAULT_FONT_LOC && pos >= location_font && pos < location_font + 0x100)
    {
        //printf("updatefont at pos %x\n", location_font);
        hw->screen_was_updated = 1;
    }
    else if (location_palette != HW_LEM1802_DEFAULT_PALETTE_LOC && pos >= location_palette && pos < location_palette + 0x10)
    {
        hw->screen_was_updated = 1;
        hw->border_was_updated = 1;
    }
}

void vm_hw_lem1802_set_border(struct lem1802_hardware* hw , uint16_t idx)
{
    // border around the screen
    hw->border_color = idx & 0xF;
    hw->border_was_updated = 1;
}

void vm_hw_lem1802_cycle(vm_t* vm, uint16_t pos, void* ud)
{
    struct lem1802_hardware* hw = (struct lem1802_hardware*)ud;

    if (hw->window_closed)
    {
        vm->exit = 1;
        return;
    }

    // blink every 60 frames (i.e. at 1 Hz)
    if (hw->blink_tick < 60)
    {
        hw->blink_tick++;
    }
    else
    {
        hw->blink_tick = 0;
        hw->blink_status = 1 - hw->blink_status;
    }

    // update the texture from the screen and font memory
    vm_hw_lem1802_update_texture(hw);
    vm_hw_lem1802_glfw_draw(hw);
}

void vm_hw_lem1802_break(vm_t* vm, uint16_t pos, void* ud)
{
    vm_hw_lem1802_cycle(vm, pos, ud);
}

void vm_hw_lem1802_interrupt(vm_t* vm, void* ud)
{
    struct lem1802_hardware* hw = (struct lem1802_hardware*)ud;
    uint16_t requested_action = vm->registers[REG_A];
    uint16_t val_b = vm->registers[REG_B];
    uint16_t idx = 0;
    uint32_t store;

    switch (requested_action)
    {
        case LEM1802_MEM_MAP_SCREEN:
            printd(LEVEL_DEBUG, "LEM1802 SCREEN MAPPED: %u.\n", val_b);
            vm_hw_lem1802_mem_set_screen(hw, val_b);
            vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
            hw->screen_was_updated = 1;
            break;

        case LEM1802_MEM_MAP_FONT:
            printd(LEVEL_DEBUG, "LEM1802 FONT MAPPED.\n");
            vm_hw_lem1802_mem_set_font(hw, val_b);
            hw->screen_was_updated = 1;
            break;

        case LEM1802_MEM_MAP_PALETTE:
            printd(LEVEL_DEBUG, "LEM1802 PALETTE MAPPED.\n");
            vm_hw_lem1802_mem_set_palette(hw, val_b);
            hw->screen_was_updated = 1;
            hw->border_was_updated = 1;
            break;

        case LEM1802_SET_BORDER_COLOR:
            printd(LEVEL_DEBUG, "LEM1802 BORDER SET.\n");
            vm_hw_lem1802_set_border(hw, val_b);
            break;

        case LEM1802_MEM_DUMP_FONT:
            printd(LEVEL_DEBUG, "LEM1802 DUMP FONT.\n");
            vm->sleep_cycles += 256;
            for (idx = 0; idx < 128; idx++)
            {
                store = vm_hw_lem1802_mem_get_font_default_representation(idx);
                vm->ram[val_b + idx * 2] = (uint16_t)(store >> 16);
                vm->ram[val_b + idx * 2 + 1] = (uint16_t)((store << 16) >> 16);
            }
            break;

        case LEM1802_MEM_DUMP_PALETTE:
            printd(LEVEL_DEBUG, "LEM1802 DUMP PALETTE.\n");
            vm->sleep_cycles += 16;
            vm_hw_lem1802_mem_dump_default_palette(vm, val_b);
            break;
    }
}


void vm_hw_lem1802_glfw_resize_handler(GLFWwindow window, int w, int h)
{
    struct lem1802_hardware* hw = (struct lem1802_hardware*) glfwGetWindowUserPointer(window);
    vm_hw_lem1802_resize_handler(hw, w, h);
}

void vm_hw_lem1802_resize_handler(struct lem1802_hardware* hw, int w, int h)
{
    if (hw != NULL && hw->context != NULL)
    {
        double original_aspect_ratio, new_aspect_ratio, x_frust, y_frust;

        original_aspect_ratio = (double)HW_LEM1802_SCREEN_TEXTURE_WIDTH / (double)HW_LEM1802_SCREEN_TEXTURE_HEIGHT;
        new_aspect_ratio = (double)w / (double)h;

        hw->vm->host->activate_context(hw->context);
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (new_aspect_ratio > original_aspect_ratio)
        {
            // black areas on the sides
            x_frust = (double)w / (h * original_aspect_ratio);
            glOrtho(-x_frust, x_frust, -1.0, 1.0, -1.0, 2.0);
        }
        else
        {
            // black areas top and bottom
            y_frust = (h * original_aspect_ratio) / ((double) w);
            glOrtho(-1.0, 1.0, -y_frust, y_frust, -1.0, 2.0);
        }

        // redraw
        vm_hw_lem1802_glfw_draw(hw);
    }
}

int vm_hw_lem1802_close(void* ud)
{
    struct lem1802_hardware* hw;
    hw = (struct lem1802_hardware*)ud;
    hw->window_closed = 1;
    return 0;
}


void vm_hw_lem1802_init_glfw(struct lem1802_hardware* hw)
{
    int width, height;

    if (hw->vm->host != NULL)
    {
        width = 4 * HW_LEM1802_SCREEN_TEXTURE_WIDTH;
        height = 4 * HW_LEM1802_SCREEN_TEXTURE_HEIGHT;
        hw->context = hw->vm->host->create_context("LEM 1802", width, height, true, hw);

        glfwSetWindowCloseCallback(hw->context, &vm_hw_glfw_close_window_callback);
        glfwSwapInterval(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        hw->texture_id = next_texture_id;
        next_texture_id++;
        glBindTexture(GL_TEXTURE_2D, hw->texture_id);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 2.0);
    }
}

void vm_hw_lem1802_glfw_draw(struct lem1802_hardware* hw)
{
    if (hw->vm->host != NULL)
    {
        hw->vm->host->activate_context(hw->context);
        if (hw->texture_has_changed)
        {
            glBindTexture(GL_TEXTURE_2D, hw->texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, HW_LEM1802_SCREEN_TEXTURE_WIDTH, HW_LEM1802_SCREEN_TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, hw->glfw_texture);
            hw->texture_has_changed = 0;
        }

        // OpenGL rendering goes here...
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);  /* enable texture mapping */
        glBindTexture(GL_TEXTURE_2D, hw->texture_id);  /* bind to our texture, has id of 13 */

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); /* lower left corner of image */
        glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner

        glTexCoord2f(0.0f, 1.0f); /* upper left corner of image */
        glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner

        glTexCoord2f(1.0f, 1.0f); /* upper right corner of image */
        glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner

        glTexCoord2f(1.0f, 0.0f); /* lower right corner of image */
        glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner

        glEnd();

        glDisable(GL_TEXTURE_2D);  /* disable texture mapping */

        // Swap front and back rendering buffers
        hw->vm->host->swap_buffers(hw->context);
    }
}

void vm_hw_lem1802_update_texture(struct lem1802_hardware* hw)
{
    uint16_t pos, val, location_screen;
    unsigned int x, y, i;

    location_screen = vm_hw_lem1802_mem_get_screen(hw);

    if (location_screen != HW_LEM1802_DISCONNECTED)
    {
        i = 0;
        if (hw->screen_was_updated)
        {
            for (pos = location_screen; pos <= location_screen + 0x17F; pos++)
            {
                val = hw->vm->ram[pos];
                x = i % HW_LEM1802_SCREEN_WIDTH;
                y = i / HW_LEM1802_SCREEN_WIDTH;
                vm_hw_lem1802_mem_put_char_to_screen(hw, val, x, y);
                i++;
            }
            hw->screen_was_updated = 0;
            hw->texture_has_changed = 1;
        }

        // update border
        if (hw->border_was_updated)
        {
            vm_hw_lem1802_mem_draw_border(hw);
            hw->border_was_updated = 0;
            hw->texture_has_changed = 1;
        }
    }
    else
    {
        if (hw->screen_was_updated)
        {
            for (i = 0; i < HW_LEM1802_SCREEN_TEXTURE_MEM_SIZE; i++)
            {
                hw->glfw_texture[i] = 0;

            }
            hw->screen_was_updated = 0;
            hw->texture_has_changed = 1;
        }
        
        if (hw->border_was_updated)
        {
            vm_hw_lem1802_mem_draw_border(hw);
            hw->border_was_updated = 0;
            hw->texture_has_changed = 1;
        }
    }
}

void vm_hw_lem1802_init(vm_t* vm)
{
    struct lem1802_hardware* hw;

    hw = malloc(sizeof(struct lem1802_hardware));
    hw->cycle_hook = 0;
    hw->write_hook = 0;
    hw->break_hook = 0;
    hw->vm = vm;

    // setting up some status values
    hw->blink_status = 1;
    hw->blink_tick = 0;
    hw->border_color = 0;
    //
    hw->border_was_updated = 1;
    hw->screen_was_updated = 1;
    hw->texture_has_changed = 1;
    hw->window_closed = 0;
    hw->context = NULL;

    // Set up the LEM1802 hardware information.
    hw->device.id = LEM1802_ID;
    hw->device.version = LEM1802_VERSION;
    hw->device.manufacturer = LEM1802_MANUFACTURER;
    hw->device.handler = &vm_hw_lem1802_interrupt;
    hw->device.free_handler = &vm_hw_lem1802_free;

    // set userdata to lem1820 struct
    hw->device.userdata = hw;

    // register hooks and hw
    hw->cycle_hook = vm_hook_register(vm, &vm_hw_lem1802_cycle, HOOK_ON_60HZ, hw);
    hw->write_hook = vm_hook_register(vm, &vm_hw_lem1802_write, HOOK_ON_WRITE, hw);
    hw->break_hook = vm_hook_register(vm, &vm_hw_lem1802_break, HOOK_ON_BREAK, hw);
    hw->hw_id = vm_hw_register(vm, &hw->device);

    hw->glfw_texture = malloc(HW_LEM1802_SCREEN_TEXTURE_MEM_SIZE);

    // Initialize the memory for LEM1802.
    vm_hw_lem1802_mem_init(hw);
    // Init GLFW and OpenGL
    vm_hw_lem1802_init_glfw(hw);

    // Set up the keyboard.
    vm_hw_keyboard_init(vm);
    glfwSetKeyCallback(hw->context, &vm_hw_keyboard_handle_key);
    glfwSetCharCallback(hw->context, &vm_hw_keyboard_handle_char);


    vm_hook_fire(hw->vm, hw->hw_id, HOOK_ON_HARDWARE_CHANGE, hw);
}

void vm_hw_lem1802_free(void* ud)
{
    struct lem1802_hardware* hw = (struct lem1802_hardware*)ud;
    // Unregister hooks.
    vm_hook_unregister(hw->vm, hw->write_hook);
    vm_hook_unregister(hw->vm, hw->cycle_hook);
    vm_hook_unregister(hw->vm, hw->break_hook);

    if(hw->vm) 
    {
        if (hw->vm->host != NULL)
            hw->vm->host->destroy_context(hw->context);
    }
    if(hw->glfw_texture)
    {
        free(hw->glfw_texture);
    }
    free(hw);
}
