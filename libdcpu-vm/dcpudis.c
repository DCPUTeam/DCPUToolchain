/**
 *
 *	File:		dcpudis.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU-VM
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Defines functions that assist
 *			in disassembling and analysing
 *			instructions.
 *
 **/

#include <stdint.h>
#include <bstring.h>
#include "dcpu.h"
#include "imap.h"
#include "dcpubase.h"
#include "dcpudis.h"
#include <lobject.h>

///
/// Disassembles a single instruction located at the specified position.
///
/// Disassembles a single instruction without affecting the state of the
/// virtual machine by saving the state of the PC and SP registers before
/// doing value resolution and then restoring them afterwards.
///
/// @param vm The virtual machine.
/// @param pos The position in RAM of the instruction to disassemble.
/// @param pretty Whether the 'pretty' substructure should be filled.  The caller is responsible for destroying the bstrings.
///
struct inst vm_disassemble(vm_t* vm, uint16_t pos, bool pretty)
{
	struct inst result;
	uint16_t pc_store = vm->pc;
	uint16_t sp_store = vm->sp;
	uint8_t debug_store = vm->debug;
	struct instruction_mapping* instmap;
	struct register_mapping* regmap;

	// Set pretty structure to NULL by default.
	result.pretty.op = NULL;
	result.pretty.a = NULL;
	result.pretty.b = NULL;

	// Set correct VM state for a resolving read.
	vm->pc = pos;
	vm->debug = false;

	// Read the basic instruction data.
	result.original.full = vm->ram[vm->pc++];
	result.original.op = INSTRUCTION_GET_OP(result.original.full);
	result.original.a = INSTRUCTION_GET_A(result.original.full);
	result.original.b = INSTRUCTION_GET_B(result.original.full);

	// Resolve values.
	if (result.original.op == OP_NONBASIC)
	{
		result.op = result.original.b;
		result.a = vm_resolve_value(vm, result.original.a, POS_A);
		result.b = 0x0;
	}
	else
	{
		result.op = result.original.op;
		result.a = vm_resolve_value(vm, result.original.a, POS_A);
		result.b = vm_resolve_value(vm, result.original.b, POS_B);
	}
	result.size = vm->pc - pos;
	result.extra[0] = 0x0;
	result.extra[1] = 0x0;
	result.next[0] = 0x0;
	result.next[1] = 0x0;
	result.used[0] = false;
	result.used[1] = false;
	if (result.size - 1 >= 1)
		result.extra[0] = vm->ram[pos + 1];
	if (result.size - 1 >= 2)
		result.extra[1] = vm->ram[pos + 2];
	if (result.size - 1 >= 2)
	{
		result.used[0] = true;
		result.used[1] = true;
		result.next[0] = vm->ram[pos + 1];
		result.next[1] = vm->ram[pos + 2];
	}
	else if (result.size - 1 >= 1 && (result.original.a == NXT || result.original.a == NXT_LIT || result.original.a == PICK || (result.original.a >= NXT_VAL_A && result.original.a <= NXT_VAL_J)))
	{
		result.used[0] = true;
		result.next[0] = vm->ram[pos + 1];
	}
	else if (result.size - 1 >= 1 && (result.original.b == NXT || result.original.b == NXT_LIT || result.original.b == PICK || (result.original.b >= NXT_VAL_A && result.original.b <= NXT_VAL_J)))
	{
		result.used[1] = true;
		result.next[1] = vm->ram[pos + 1];
	}

	// Work out pretty values if required.
	instmap = get_instruction_by_value(result.original.op, result.original.b);
	if (pretty && instmap != NULL)
	{
		// Work out the instruction name.
		result.pretty.op = bfromcstr(instmap->name);

		// Work out the a parameter.
		regmap = get_register_by_value(result.original.a);
		if (result.original.a >= REG_A && result.original.a <= REG_J)
			result.pretty.a = bfromcstr(regmap->name);
		else if (result.original.a >= VAL_A && result.original.a <= VAL_J)
			result.pretty.a = bformat("[%s]", regmap->name);
		else if (result.original.a >= NXT_VAL_A && result.original.a <= NXT_VAL_J)
			result.pretty.a = bformat("[%s+0x%04X]", regmap->name, result.next[0]);
		else if (result.original.a == PUSH_POP)
			result.pretty.a = bfromcstr("POP");
		else if (result.original.a == PEEK)
			result.pretty.a = bfromcstr("[SP]");
		else if (result.original.a == PICK)
			result.pretty.a = bformat("[SP+0x%04X]", result.next[0]);
		else if (result.original.a >= SP && result.original.a <= EX)
			result.pretty.a = bfromcstr(regmap->name);
		else if (result.original.a == NXT)
			result.pretty.a = bformat("[0x%04X]", result.next[0]);
		else if (result.original.a == NXT_LIT)
			result.pretty.a = bformat("0x%04X", result.next[0]);
		else if (result.original.a >= 0x20 && result.original.a <= 0x3F)
			result.pretty.a = bformat("0x%04X", result.original.a - 0x21);
		else
			result.pretty.a = bfromcstr("???");

		// Only fill the b parameter if the opcode was basic.
		if (result.pretty.op != OP_NONBASIC)
		{
			regmap = get_register_by_value(result.original.b);
			if (result.original.b >= REG_A && result.original.b <= REG_J)
				result.pretty.b = bfromcstr(regmap->name);
			else if (result.original.b >= VAL_A && result.original.b <= VAL_J)
				result.pretty.b = bformat("[%s]", regmap->name);
			else if (result.original.b >= NXT_VAL_A && result.original.b <= NXT_VAL_J)
				result.pretty.b = bformat("[%s+0x%04X]", regmap->name, result.next[1]);
			else if (result.original.b == PUSH_POP)
				result.pretty.b = bfromcstr("PUSH");
			else if (result.original.b == PEEK)
				result.pretty.b = bfromcstr("[SP]");
			else if (result.original.b == PICK)
				result.pretty.b = bformat("[SP+0x%04X]", result.next[1]);
			else if (result.original.b >= SP && result.original.b <= EX)
				result.pretty.b = bfromcstr(regmap->name);
			else if (result.original.b == NXT)
				result.pretty.b = bformat("[0x%04X]", result.next[1]);
			else if (result.original.b == NXT_LIT)
				result.pretty.b = bformat("0x%04X", result.next[1]);
			else if (result.original.b >= 0x20 && result.original.b <= 0x3F)
				result.pretty.b = bformat("0x%04X", result.original.b - 0x21);
			else
				result.pretty.b = bfromcstr("???");
		}
	}

	// Restore state.
	vm->pc = pc_store;
	vm->sp = sp_store;
	vm->debug = debug_store;

	// Return instruction.
	return result;
}