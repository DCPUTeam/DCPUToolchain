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
#include "dcpu.h"
#include "hwlem1802.h"

#define HW_LEM1802_DISCONNECTED				0
#define HW_LEM1802_DEFAULT_FONT_LOC			0
#define HW_LEM1802_DEFAULT_PALETTE_LOC			0
#define HW_LEM1802_INTERNAL				0
#define HW_LEM1802_SCREEN_WIDTH				32
#define HW_LEM1802_SCREEN_HEIGHT			12
#define HW_LEM1802_SCREEN_MEMSIZE			(32 * 12)
#define HW_LEM1802_SCREEN_TEXTURE_HEIGHT		96
#define HW_LEM1802_SCREEN_TEXTURE_WIDTH			128
#define HW_LEM1802_FONT_MEMSIZE				(128 * 2)
#define HW_LEM1802_FONT_CHAR_ADDRESSABLE_WIDTH		4
#define HW_LEM1802_FONT_CHAR_ADDRESSABLE_HEIGHT		8
#define HW_LEM1802_FONT_NUM_CHARS			128
#define HW_LEM1802_FONT_IMG_COLS			32
#define HW_LEM1802_FONT_IMG_WIDTH			128
#define HW_LEM1802_FONT_RGB_COOR(x, y) (3*((y*HW_LEM1802_FONT_IMG_WIDTH)+x))
#define HW_LEM1802_SCREEN_RGB_COOR(x, y) (3*((y*HW_LEM1802_SCREEN_TEXTURE_WIDTH)+x))


void vm_hw_lem1802_mem_init(struct lem1802_hardware* hw);
void vm_hw_lem1802_mem_free(vm_t* vm);
void vm_hw_lem1802_mem_set_palette(struct lem1802_hardware* hw, uint16_t pos);
void vm_hw_lem1802_mem_dump_default_palette(vm_t* vm, uint16_t pos);
void vm_hw_lem1802_mem_get_palette_color(struct lem1802_hardware* hw, uint16_t idx, unsigned char * rgb);
void vm_hw_lem1802_mem_get_default_palette_color(uint16_t idx, unsigned char * rgb);
void vm_hw_lem1802_mem_set_font(struct lem1802_hardware* hw, uint16_t pos);
uint16_t vm_hw_lem1802_mem_get_font(struct lem1802_hardware* hw);
void vm_hw_lem1802_mem_load_default_font();
void vm_hw_lem1802_mem_put_char_to_screen(struct lem1802_hardware* hw, uint16_t val, unsigned int screenX, unsigned int screenY);
uint32_t vm_hw_lem1802_mem_get_font_default_representation(uint16_t idx);

void vm_hw_lem1802_mem_set_screen(struct lem1802_hardware* hw, uint16_t pos);
uint16_t vm_hw_lem1802_mem_get_screen(struct lem1802_hardware* hw);

#endif

