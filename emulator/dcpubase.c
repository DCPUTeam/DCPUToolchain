//
// DCPU Emulator by James Rhodes
//
// Virtual machine - core instructions.
//

#include <stdarg.h>
#include <stdio.h>
#include "dcpubase.h"
#include "dcpuops.h"

void vm_halt(vm_t* vm, const char* message, ...)
{
    va_list argptr;
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
	vm->halted = true;
	return;
}

uint16_t vm_consume_word(vm_t* vm)
{
	uint16_t v = vm->ram[vm->pc];
	vm->pc++;
	return v;
}

uint16_t vm_resolve_value(vm_t* vm, uint16_t val)
{
	uint16_t t;
	switch (val)
	{
	case REG_A:
	case REG_B:
	case REG_C:
	case REG_X:
	case REG_Y:
	case REG_Z:
	case REG_I:
	case REG_J:
		return vm->registers[val];
	case VAL_A:
	case VAL_B:
	case VAL_C:
	case VAL_X:
	case VAL_Y:
	case VAL_Z:
	case VAL_I:
	case VAL_J:
		return vm->ram[(uint16_t)vm->registers[val - VAL_A]];
	case NXT_VAL_A:
	case NXT_VAL_B:
	case NXT_VAL_C:
	case NXT_VAL_X:
	case NXT_VAL_Y:
	case NXT_VAL_Z:
	case NXT_VAL_I:
	case NXT_VAL_J:
		return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm_consume_word(vm))];
	case POP:
		t = vm->sp++;
		return vm->ram[t];
	case PEEK:
		t = vm->sp;
		return vm->ram[t];
	case PUSH:
		t = --vm->sp;
		return vm->ram[t];
	case SP:
		return vm->sp;
	case PC:
		return vm->pc;
	case O:
		return vm->o;
	case NXT:
		return vm->ram[(uint16_t)vm_consume_word(vm)];
	case NXT_LIT:
		return vm_consume_word(vm);
	default:
		return val - 0x20;
	}
}

void vm_print_op(const char* opname, vm_t* vm, uint16_t a, uint16_t b)
{
	if (vm->skip)
		printf("(skipped) %s 0x%04X 0x%04X", opname, a, b);
	else
		printf("%s 0x%04X 0x%04X", opname, a, b);
}

void vm_print_op_nonbasic(const char* opname, vm_t* vm, uint16_t a)
{
	if (vm->skip)
		printf("(skipped) %s 0x%04X", opname, a);
	else
		printf("%s 0x%04X", opname, a);
}

void vm_cycle(vm_t* vm)
{
	uint16_t instruction = vm_consume_word(vm);
	uint16_t op = INSTRUCTION_GET_OP(instruction);
	uint16_t a = INSTRUCTION_GET_A(instruction);
	uint16_t b = INSTRUCTION_GET_B(instruction);
	uint16_t t;

	switch (op)
	{
	case OP_SET:
		vm_print_op("SET", vm, a, b);
		vm_op_set(vm, a, b);
		break;
	case OP_ADD:
		vm_print_op("ADD", vm, a, b);
		vm_op_add(vm, a, b);
		break;
	case OP_SUB:
		vm_print_op("SUB", vm, a, b);
		vm_op_sub(vm, a, b);
		break;
	case OP_MUL:
		vm_print_op("MUL", vm, a, b);
		vm_op_mul(vm, a, b);
		break;
	case OP_DIV:
		vm_print_op("DIV", vm, a, b);
		vm_op_div(vm, a, b);
		break;
	case OP_MOD:
		vm_print_op("MOD", vm, a, b);
		vm_op_mod(vm, a, b);
		break;
	case OP_SHL:
		vm_print_op("SHL", vm, a, b);
		vm_op_shl(vm, a, b);
		break;
	case OP_SHR:
		vm_print_op("SHR", vm, a, b);
		vm_op_shr(vm, a, b);
		break;
	case OP_AND:
		vm_print_op("AND", vm, a, b);
		vm_op_and(vm, a, b);
		break;
	case OP_BOR:
		vm_print_op("BOR", vm, a, b);
		vm_op_bor(vm, a, b);
		break;
	case OP_XOR:
		vm_print_op("XOR", vm, a, b);
		vm_op_xor(vm, a, b);
		break;
	case OP_IFE:
		vm_print_op("IFE", vm, a, b);
		vm_op_ife(vm, a, b);
		break;
	case OP_IFN:
		vm_print_op("IFN", vm, a, b);
		vm_op_ifn(vm, a, b);
		break;
	case OP_IFG:
		vm_print_op("IFG", vm, a, b);
		vm_op_ifg(vm, a, b);
		break;
	case OP_IFB:
		vm_print_op("IFB", vm, a, b);
		vm_op_ifb(vm, a, b);
		break;
	case OP_NONBASIC:
		t = a;
		a = b;
		switch (t)
		{
		case NBOP_JSR:
			vm_print_op_nonbasic("JSR", vm, a);
			vm_op_jsr(vm, a);
			break;
		default:
			vm_halt(vm, "Invalid non-basic opcode %u.", t);
			break;
		}
		break;
	default:
		vm_halt(vm, "Invalid opcode %u.", op);
		break;
	}
	printf("\n");
}