/**

	File:		dcpuops.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes
			Aaron Miller

	Description:	Handles opcode instructions in the
			virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <libtcod.h>
#include <stdio.h>
#include "dcpubase.h"
#include "dcpuops.h"
#include "dcpuhook.h"
#include "hw.h"

#define VM_CHECK_ARITHMETIC_FLOW(op, val_a, val_b) \
	if ((int32_t)val_a op (int32_t)val_b < (int32_t)0) \
		vm->ex = AR_UNDERFLOW; \
	else if ((int32_t)val_a op (int32_t)val_b > (int32_t)AR_MAX) \
		vm->ex = AR_OVERFLOW; \
	else \
		vm->ex = AR_NOFLOW;

#define VM_CHECK_ARITHMETIC_FLOW_EX(op, val_a, val_b, val_ex) \
	if ((int32_t)val_a op (int32_t)val_b + (int32_t)val_ex < (int32_t)0) \
		vm->ex = AR_UNDERFLOW; \
	else if ((int32_t)val_a op (int32_t)val_b + (int32_t)val_ex > (int32_t)AR_MAX) \
		vm->ex = AR_OVERFLOW; \
	else \
		vm->ex = AR_NOFLOW;

#define VM_SKIP_RESET if(vm->skip) {vm->skip = false; return;}

#define VM_HOOK_FIRE(var) \
	if (var >= (uint16_t*)&vm->ram && var < (uint16_t*)&vm->ram + 0x10000) \
		vm_hook_fire(vm, (uint16_t)(var - (uint16_t*)&vm->ram), HOOK_ON_WRITE);

#define OP_NUM_CYCLES(count) vm->sleep_cycles += (count - 1);

uint16_t irqs = 0x0;

uint16_t* vm_internal_get_store(vm_t* vm, uint16_t loc, uint8_t pos)
{
	uint16_t t;

	// Don't modify the state of the program if
	// we're skipping.
	if (vm->skip)
	{
		if ((loc >= NXT_VAL_A && loc <= NXT_VAL_J) || loc == NXT)
			vm_consume_word(vm);

		return &vm->dummy;
	}

	// Otherwise, run normally.
	if (loc >= REG_A && loc <= REG_J)
		return &vm->registers[loc];
	else if (loc >= VAL_A && loc <= VAL_J)
		return &vm->ram[vm->registers[loc - VAL_A]];
	else if (loc >= NXT_VAL_A && loc <= NXT_VAL_J)
	{
		vm->sleep_cycles += 1; // Resolving this costs an additional cycle
		return &vm->ram[(uint16_t)(vm->registers[loc - NXT_VAL_A] + vm_consume_word(vm))];
	}
	else if (loc == PUSH_POP)
	{
		if (pos == POS_A)
			t = vm->sp++;
		else if (pos == POS_B)
			t = --vm->sp;
		else
			t = vm->sp;

		return &vm->ram[t];
	}
	else if (loc == PEEK)
		return &vm->ram[vm->sp];
	else if (loc == PICK)
	{
		vm->sleep_cycles += 1; // Resolving this costs an additional cycle
		return &vm->ram[(uint16_t)(vm->sp + vm_consume_word(vm))];
	}
	else if (loc == IA)
		return &vm->ia;
	else if (loc == SP)
		return &vm->sp;
	else if (loc == PC)
		return &vm->pc;
	else if (loc == EX)
		return &vm->ex;
	else if (loc == NXT)
	{
		vm->sleep_cycles += 1; //Resolving this costs an additional cycle
		return &vm->ram[vm_consume_word(vm)];
	}
	else
		return &vm->dummy; // Dummy position for assignments that silently fail.
}

// Sometimes an instruction will get the value of 'a' for a second
// time (such as in the case of ADD).  We need to make sure that
// if the value of 'a' modified the VM state in vm_internal_get_store
// that we don't modify the state again (for example, not incrementing
// PC for NXT).
uint16_t vm_resolve_value_once(vm_t* vm, uint16_t val, uint8_t pos)
{
	switch (val)
	{
		case NXT:
			return vm->ram[(uint16_t)vm->ram[(uint16_t)(vm->pc - 1)]];

		case PUSH_POP:
			return vm->ram[vm->sp];

		case PICK:
			return vm->ram[(uint16_t)(vm->sp + vm->ram[(uint16_t)(vm->pc - 1)])];

		case NXT_VAL_A:
		case NXT_VAL_B:
		case NXT_VAL_C:
		case NXT_VAL_X:
		case NXT_VAL_Y:
		case NXT_VAL_Z:
		case NXT_VAL_I:
		case NXT_VAL_J:
			return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm->ram[(uint16_t)(vm->pc - 1)])];

		default:
			return vm_resolve_value(vm, val, pos);
	}
}

void vm_op_set(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_a;
	VM_HOOK_FIRE(store_b);
	vm->skip = false;
}

void vm_op_add(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	store_b = vm_internal_get_store(vm, b, POS_B);
	val_b = *store_b;
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;
	*store_b = val_b + val_a;
	VM_CHECK_ARITHMETIC_FLOW(+, val_b, val_a);
	VM_HOOK_FIRE(store_b);
}

void vm_op_sub(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;
	*store_b = val_b - val_a;
	VM_CHECK_ARITHMETIC_FLOW(-, val_b, val_a);
	VM_HOOK_FIRE(store_b);
}

void vm_op_mul(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;
	*store_b = val_b * val_a;
	vm->ex = ((val_b * val_a) >> 16) & 0xffff;
	VM_HOOK_FIRE(store_b);
}

void vm_op_mli(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	int16_t val_b;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = (int16_t)vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;
	*store_b = (uint16_t)(val_b * val_a);
	vm->ex = ((val_b * val_a) >> 16) & 0xffff;
	VM_HOOK_FIRE(store_b);
}

void vm_op_div(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;

	if (val_a != 0)
	{
		*store_b = val_b / val_a;
		vm->ex = ((val_b << 16) / val_a) & 0xffff;
	}
	else
	{
		*store_b = 0;
		vm->ex = 0;
	}

	VM_HOOK_FIRE(store_b);
}

void vm_op_dvi(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	int16_t val_b;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = (int16_t)vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;

	if (val_a != 0)
	{
		*store_b = val_b / val_a;
		vm->ex = ((val_b << 16) / val_a) & 0xffff;
	}
	else
	{
		*store_b = 0;
		vm->ex = 0;
	}

	VM_HOOK_FIRE(store_b);
}

void vm_op_mdi(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	uint16_t val_b;
	int16_t val_b_signed;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;

	// compute 2s complement
	if(val_b > 2^8) 
		val_b_signed = 0 - (0x10000 - val_b);
	else
		val_b_signed = val_b;
		
	if (val_a != 0)
		*store_b = val_b_signed % val_a;
	else
		*store_b = 0;

	VM_HOOK_FIRE(store_b);
}

void vm_op_mod(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;

	if (val_a != 0)
		*store_b = val_b % val_a;
	else
		*store_b = 0;

	VM_HOOK_FIRE(store_b);
}


void vm_op_and(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b & val_a;
	VM_HOOK_FIRE(store_b);
}

void vm_op_bor(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b | val_a;
	VM_HOOK_FIRE(store_b);
}

void vm_op_xor(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b ^ val_a;
	VM_HOOK_FIRE(store_b);
}

void vm_op_shr(vm_t* vm, uint16_t a, uint16_t b)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b >> val_a;
	vm->ex = ((val_b << 16) >> val_a) & 0xffff;
	VM_HOOK_FIRE(store_b);
}

void vm_op_asr(vm_t* vm, uint16_t a, uint16_t b)
{
	// TODO: This may not infact be correct.  C uses
	// arithmetic shifts if the left-hand value is
	// signed, however, we still need to make sure that the
	// excess register is completely compliant.
	int16_t val_b, val_a;
	uint16_t* store_b;
	val_a = (int16_t)vm_resolve_value(vm, a, POS_A);
	val_b = (int16_t)vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b >> val_a;
	vm->ex = ((val_b << 16) >> val_a) & 0xffff;
	VM_HOOK_FIRE(store_b);
}

void vm_op_shl(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(1);

	VM_SKIP_RESET;
	*store_b = val_b << val_a;
	vm->ex = ((val_b << val_a) >> 16) & 0xffff;
	VM_HOOK_FIRE(store_b);
}

void vm_op_ifb(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !((val_b & val_a) != 0);
}

void vm_op_ifc(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !((val_b & val_a) == 0);
}

void vm_op_ife(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !(val_b == val_a);
}

void vm_op_ifn(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !(val_b != val_a);
}

void vm_op_ifg(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !(val_b > val_a);
}

void vm_op_ifa(vm_t* vm, uint16_t b, uint16_t a)
{
	int16_t val_b, val_a;
	val_a = (int16_t)vm_resolve_value(vm, a, POS_A);
	val_b = (int16_t)vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !(val_b > val_a);
}

void vm_op_ifl(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a;
	val_b = vm_resolve_value(vm, b, POS_B);
	val_a = vm_resolve_value(vm, a, POS_A);

	VM_SKIP_RESET;
	vm->skip = !(val_b < val_a);
}

void vm_op_ifu(vm_t* vm, uint16_t b, uint16_t a)
{
	int16_t val_b, val_a;
	val_a = (int16_t)vm_resolve_value(vm, a, POS_A);
	val_b = (int16_t)vm_resolve_value(vm, b, POS_B);

	VM_SKIP_RESET;
	vm->skip = !(val_b < val_a);
}

void vm_op_adx(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a, val_ex;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	val_ex = vm->ex;
	VM_SKIP_RESET;
	*store_b = val_b + val_a + val_ex;
	VM_CHECK_ARITHMETIC_FLOW_EX(+, val_b, val_a, val_ex);
	VM_HOOK_FIRE(store_b);
}

void vm_op_sbx(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_b, val_a, val_ex;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	val_b = vm_resolve_value_once(vm, b, POS_B);
	store_b = vm_internal_get_store(vm, b, POS_B);
	OP_NUM_CYCLES(3);

	val_ex = vm->ex;
	VM_SKIP_RESET;
	*store_b = val_b - val_a + val_ex;
	VM_CHECK_ARITHMETIC_FLOW_EX(-, val_b, val_a, val_ex);
	VM_HOOK_FIRE(store_b);
}

void vm_op_sti(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	store_b = vm_internal_get_store(vm, b, POS_B);

	VM_SKIP_RESET;
	*store_b = val_a;
	VM_HOOK_FIRE(store_b);
	vm->registers[REG_I]++;
	vm->registers[REG_J]++;
	VM_HOOK_FIRE(&vm->registers[REG_I]);
	VM_HOOK_FIRE(&vm->registers[REG_J]);
}

void vm_op_std(vm_t* vm, uint16_t b, uint16_t a)
{
	uint16_t val_a;
	uint16_t* store_b;
	val_a = vm_resolve_value(vm, a, POS_A);
	store_b = vm_internal_get_store(vm, b, POS_B);

	VM_SKIP_RESET;
	*store_b = val_a;
	VM_HOOK_FIRE(store_b);
	vm->registers[REG_I]--;
	vm->registers[REG_J]--;
	VM_HOOK_FIRE(&vm->registers[REG_I]);
	VM_HOOK_FIRE(&vm->registers[REG_J]);
}

void vm_op_jsr(vm_t* vm, uint16_t a)
{
	uint16_t new_pc = vm_resolve_value(vm, a, POS_A);
	uint16_t t;
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;
	t = --vm->sp;
	vm->ram[t] = vm->pc;
	vm->pc = new_pc;
}

void vm_op_hcf(vm_t* vm, uint16_t a)
{
	OP_NUM_CYCLES(9);
	vm->halted = true; // TODO: make it do crazy shit
}

void vm_op_int(vm_t* vm, uint16_t a)
{
	OP_NUM_CYCLES(4);

	if (vm->queue_interrupts == 1)
	{
		irqs++;
		vm->irq[irqs] = a;
	}
	else
	{
		vm_interrupt(vm, a);
	}
}

void vm_op_iag(vm_t* vm, uint16_t a)
{
	VM_SKIP_RESET;
	vm_op_set(vm, a, IA);
}

void vm_op_ias(vm_t* vm, uint16_t a)
{
	VM_SKIP_RESET;

	if (a != 0)
		vm->queue_interrupts = 1;

	vm_op_set(vm, IA, a);
}

void vm_op_rfi(vm_t* vm, uint16_t a)
{
	OP_NUM_CYCLES(3);

	VM_SKIP_RESET;

	vm->queue_interrupts = 0;
	vm->registers[REG_A] = vm->ram[++vm->sp];
	vm->registers[PC] = vm->ram[++vm->sp];
	VM_HOOK_FIRE(&vm->registers[REG_A]);
}

void vm_op_iaq(vm_t* vm, uint16_t a)
{
	uint16_t i = 0x0;
	uint16_t val_a = vm_resolve_value(vm, a, POS_A);
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;

	if (val_a == 0)
	{
		for (i = 0; i < irqs; i++)
		{
			vm_interrupt(vm, vm->irq[i]);
		}

		irqs = 0;
		vm->queue_interrupts = 0;
	}
	else
	{
		vm->queue_interrupts = 1;
	}
}

void vm_op_hwn(vm_t* vm, uint16_t a)
{
	uint16_t* store_a = vm_internal_get_store(vm, a, POS_A);
	OP_NUM_CYCLES(2);

	VM_SKIP_RESET;

	*store_a = vm_hw_count(vm);

	if (vm->debug) printf("\nhwn: %d devices", *store_a);

	VM_HOOK_FIRE(store_a);
	vm->skip = false;
}

void vm_op_hwq(vm_t* vm, uint16_t a)
{
	hw_t queried_device;
	uint16_t* store_a = vm_internal_get_store(vm, REG_A, POS__);
	uint16_t* store_b = vm_internal_get_store(vm, REG_B, POS__);
	uint16_t* store_c = vm_internal_get_store(vm, REG_C, POS__);
	uint16_t* store_x = vm_internal_get_store(vm, REG_X, POS__);
	uint16_t* store_y = vm_internal_get_store(vm, REG_Y, POS__);

	uint16_t val_a = vm_resolve_value(vm, a, POS_A);
	OP_NUM_CYCLES(4);

	VM_SKIP_RESET;

	queried_device = vm_hw_get_device(vm, val_a);

	if (vm->debug) printf("\nhwq: index %d %08X", val_a, queried_device.id);

	*store_a = (queried_device.id & 0x0000FFFF) >>	0;
	*store_b = (queried_device.id & 0xFFFF0000) >> 16;
	*store_c = queried_device.version;
	*store_x = (queried_device.manufacturer & 0x0000FFFF) >>  0;
	*store_y = (queried_device.manufacturer & 0xFFFF0000) >> 16;

	VM_HOOK_FIRE(store_a);
	VM_HOOK_FIRE(store_b);
	VM_HOOK_FIRE(store_c);
	VM_HOOK_FIRE(store_x);
	VM_HOOK_FIRE(store_y);
	vm->skip = false;
}

void vm_op_hwi(vm_t* vm, uint16_t a)
{
	uint16_t val_a = vm_resolve_value(vm, a, POS_A);
	OP_NUM_CYCLES(4);

	VM_SKIP_RESET;
	vm_hw_interrupt(vm, val_a);
}
