/**

	File:			dcpuops.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes
					Aaron Miller

	Description:	Handles opcode instructions in the
					virtual machine.

**/

#include "dcpubase.h"
#include "dcpuops.h"

#define VM_CHECK_ARITHMETIC_FLOW(op, val_a, val_b) \
	if ((int32_t)val_a op (int32_t)val_b < (int32_t)0) \
		vm->o = AR_UNDERFLOW; \
	else if ((int32_t)val_a op (int32_t)val_b > (int32_t)AR_MAX) \
		vm->o = AR_OVERFLOW; \
	else \
		vm->o = AR_NOFLOW;

#define VM_SKIP_RESET if(vm->skip) {vm->skip = false; return;}

uint16_t* vm_internal_get_store(vm_t* vm, uint16_t loc)
{
	// Don't modify the state of the program if
	// we're skipping.
	if (vm->skip)
	{
		if((loc >= NXT_VAL_A && loc <= NXT_VAL_J) || loc == NXT)
			vm_consume_word(vm);
		return &vm->dummy;
	}

	// Otherwise, run normally.
	if (loc >= REG_A && loc <= REG_J)
		return &vm->registers[loc];
	else if (loc >= VAL_A && loc <= VAL_J)
		return &vm->ram[(uint16_t)vm->registers[loc - VAL_A]];
	else if (loc >= NXT_VAL_A && loc <= NXT_VAL_J)
		return &vm->ram[(uint16_t)(vm->registers[loc - NXT_VAL_A] + vm_consume_word(vm))];
	else if (loc == POP)
	{
		uint16_t t = vm->sp++;
		return &vm->ram[t];
	}
	else if (loc == PEEK)
	{
		uint16_t t = vm->sp;
		return &vm->ram[t];
	}
	else if (loc == PUSH)
	{
		uint16_t t = --vm->sp;
		return &vm->ram[t];
	}
	else if (loc == SP)
		return &vm->sp;
	else if (loc == PC)
		return &vm->pc;
	else if (loc == O)
		return &vm->o;
	else if (loc == NXT)
		return &vm->ram[(uint16_t)vm_consume_word(vm)];
	else
		return &vm->dummy; // Dummy position for assignments that silently fail.
}

// Sometimes an instruction will get the value of 'a' for a second
// time (such as in the case of ADD).  We need to make sure that
// if the value of 'a' modified the VM state in vm_internal_get_store
// that we don't modify the state again (for example, not incrementing
// PC for NXT).
uint16_t vm_resolve_value_once(vm_t* vm, uint16_t val)
{
	switch (val)
	{
	case NXT:
		return vm->ram[(uint16_t)vm->pc];
	case POP:
	case PUSH:
		return vm->ram[(uint16_t)vm->sp];
	case NXT_VAL_A:
	case NXT_VAL_B:
	case NXT_VAL_C:
	case NXT_VAL_X:
	case NXT_VAL_Y:
	case NXT_VAL_Z:
	case NXT_VAL_I:
	case NXT_VAL_J:
		return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm->ram[vm->pc])];
	default:
		return vm_resolve_value(vm, val);
	}
}

void vm_op_set(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_b = vm_resolve_value(vm, b);
	*store_a = val_b;
	vm->skip = false;
}

void vm_op_add(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a + val_b;
	VM_CHECK_ARITHMETIC_FLOW(+, val_a, val_b);
}

void vm_op_sub(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a - val_b;
	VM_CHECK_ARITHMETIC_FLOW(-, val_a, val_b);
}

void vm_op_mul(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a * val_b;
	vm->o = ((val_a * val_b) >> 16) & 0xffff;
}

void vm_op_div(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	if (val_b != 0)
	{
		*store_a = val_a / val_b;
		vm->o = ((val_a << 16) / val_b) & 0xffff;
	}
	else
	{
		*store_a = 0;
		vm->o = 0;
	}
}

void vm_op_mod(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	if (val_b != 0)
		*store_a = val_a % val_b;
	else
		*store_a = 0;
}

void vm_op_shl(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a << val_b;
	vm->o = ((val_a << val_b) >> 16) & 0xffff;
}

void vm_op_shr(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a >> val_b;
	vm->o = ((val_a << 16) >> val_b) & 0xffff;
}

void vm_op_and(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a & val_b;
}

void vm_op_bor(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a | val_b;
}

void vm_op_xor(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	uint16_t* store_a = vm_internal_get_store(vm, a);
	val_a = vm_resolve_value_once(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	*store_a = val_a ^ val_b;
}

void vm_op_ife(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	val_a = vm_resolve_value(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	vm->skip = !(val_a == val_b);
}

void vm_op_ifn(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	val_a = vm_resolve_value(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	vm->skip = !(val_a != val_b);
}

void vm_op_ifg(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	val_a = vm_resolve_value(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	vm->skip = !(val_a > val_b);
}

void vm_op_ifb(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_a, val_b;
	val_a = vm_resolve_value(vm, a);
	val_b = vm_resolve_value(vm, b);
	VM_SKIP_RESET;
	vm->skip = !((val_a & val_b) != 0);
}

void vm_op_jsr(vm_t* vm, uint16_t a)
{
	uint16_t new_pc = vm_resolve_value(vm, a);
	uint16_t t;
	VM_SKIP_RESET;
	t = --vm->sp;
	vm->ram[t] = vm->pc;
	vm->pc = new_pc;
}
