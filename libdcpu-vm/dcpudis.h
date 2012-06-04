/**
 * 
 *	File:		dcpudis.h
 * 
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU-VM
 * 
 *	Authors:	James Rhodes
 * 
 *	Description:	Declares structures and functions that
 *			assist in disassembling and analysing
 *			instructions.
 * 
 **/

#ifndef __DCPUDIS_H
#define __DCPUDIS_H

#include <stdint.h>
#include <bstring.h>
#include "dcpu.h"

///
/// Represents an instruction that was observed or
/// resolved without affecting the virtual machine.
/// Internally this structure is *not* used to process
/// instructions, it exists only to allow easy
/// inspection of instructions without affecting the
/// state of the virtual machine.
///
struct inst
{
	struct
	{
		uint16_t full;
		uint8_t op;
		uint8_t a;
		uint8_t b;
	} original;
	struct
	{
		bstring op;
		bstring a;
		bstring b;
	} pretty;
	uint8_t op;
	uint16_t a;
	uint16_t b;
	uint16_t size;
	uint16_t extra[2];
	uint16_t next[2];
	uint8_t used[2];
};

struct inst vm_disassemble(vm_t* vm, uint16_t pos, bool pretty);

#endif
