/**

	File:		lem1802mem.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-VM

	Authors:	James Rhodes
			José Manuel Díez
			Tyrel Haveman

	Description:	Handles internal LEM1802 memory.

**/

#ifndef __DCPU_HW_LEM1802_MEM_H
#define __DCPU_HW_LEM1802_MEM_H

#include <stdint.h>
#include <libtcod.h>
#include "dcpu.h"

#define HW_LEM1802_DISCONNECTED				0
#define HW_LEM1802_INTERNAL				0
#define HW_LEM1802_SCREEN_WIDTH				32
#define HW_LEM1802_SCREEN_HEIGHT			12
#define HW_LEM1802_SCREEN_MEMSIZE			(32 * 12)
#define HW_LEM1802_FONT_MEMSIZE				(128 * 2)
#define HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH		4
#define HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT		8

void vm_hw_lem1802_mem_init(vm_t* vm);
void vm_hw_lem1802_mem_free(vm_t* vm);
void vm_hw_lem1802_mem_set_palette(vm_t* vm, uint16_t pos);
TCOD_color_t vm_hw_lem1802_mem_get_palette_color(vm_t* vm, uint16_t idx);
void vm_hw_lem1802_mem_set_font(vm_t* vm, uint16_t pos);
uint16_t vm_hw_lem1802_mem_get_font();
uint16_t vm_hw_lem1802_mem_get_font_char_width();
uint16_t vm_hw_lem1802_mem_get_font_char_height();
TCOD_image_t vm_hw_lem1802_mem_get_font_image();
void vm_hw_lem1802_mem_set_screen(vm_t* vm, uint16_t pos);
uint16_t vm_hw_lem1802_mem_get_screen();

#endif