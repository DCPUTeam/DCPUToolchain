/**

	File:		dcpubase.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-vm

	Authors:	James Rhodes
			Aaron Miller

	Description:	Handles core functionality of
			the virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <imap.h>
#include <sys/time.h>

#include "dcpubase.h"
#include "dcpuops.h"
#include "dcpuhook.h"

void vm_halt(vm_t* vm, const char* message, ...)
{
	va_list argptr;
	va_start(argptr, message);
	vprintd(LEVEL_DEFAULT, message, argptr);
	va_end(argptr);
	vm->halted = true;
	return;
}

void vm_interrupt(vm_t* vm, uint16_t msgid)
{
	uint16_t save;

	if (vm->queue_interrupts)
	{
		if (vm->irq_count >= INTERRUPT_MAX)
		{
			printd(LEVEL_ERROR, "interrupt queue hit maximum number (halted VM)\n");
			vm->halted = true;
		}
		else
		{
			vm->irq[vm->irq_count++] = msgid;
			printd(LEVEL_DEBUG, "appending interrupt %u to interrupt queue\n", msgid);
		}
	}
	else
	{
		if (vm->ia == 0)
		{
			printd(LEVEL_DEBUG, "ignoring request to fire interrupt %u\n", msgid);
			return;
		}

		save = vm->pc;
		vm->ram[--vm->sp] = vm->pc;
		vm->ram[--vm->sp] = vm->registers[REG_A];
		vm->registers[REG_A] = msgid;
		vm->pc = vm->ia;
		printd(LEVEL_DEBUG, "turning on interrupt queue\n");
		vm->queue_interrupts = true;
		printd(LEVEL_DEBUG, "executing interrupt %u right now\n", msgid);
		vm_hook_fire(vm, save, HOOK_ON_INTERRUPT);
	}
}

uint16_t vm_consume_word(vm_t* vm)
{
	uint16_t v = vm->ram[vm->pc];
	vm->pc++;
	return v;
}

uint16_t vm_resolve_value(vm_t* vm, uint16_t val, uint8_t pos)
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
			return vm->ram[vm->registers[val - VAL_A]];

		case NXT_VAL_A:
		case NXT_VAL_B:
		case NXT_VAL_C:
		case NXT_VAL_X:
		case NXT_VAL_Y:
		case NXT_VAL_Z:
		case NXT_VAL_I:
		case NXT_VAL_J:
			return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm_consume_word(vm))];

		case PUSH_POP:
			if (vm->skip) return vm->dummy;

			if (pos == POS_A)
				t = vm->sp++;
			else if (pos == POS_B)
				t = --vm->sp;
			else
				// Probably should handle if pos is neither
				// A nor B to catch internal errors.
				t = vm->sp;

			return vm->ram[t];

		case PEEK:
			t = vm->sp;
			return vm->ram[t];

		case PICK:
			t = vm->sp;
			return vm->ram[(uint16_t)(t + vm_consume_word(vm))];

		case IA:
			return vm->ia;

		case SP:
			return vm->sp;

		case PC:
			return vm->pc;

		case EX:
			return vm->ex;

		case NXT:
			t = vm->ram[vm_consume_word(vm)];

			if (vm->debug || vm->dump != NULL) fprintf(vm->dump == NULL ? stdout : vm->dump, " (0x%04X) ", t);

			return t;

		case NXT_LIT:
			t = vm_consume_word(vm);

			if (vm->debug || vm->dump != NULL) fprintf(vm->dump == NULL ? stdout : vm->dump, " (0x%04X) ", t);

			return t;

		default:
			return val - 0x21;
	}
}

void vm_print_op(const char* opname, vm_t* vm, uint16_t a, uint16_t b)
{
	FILE* target = stdout;
	if (!vm->debug && vm->dump == NULL)
		return;
	if (vm->dump != NULL)
		target = vm->dump;

	if (vm->skip)
	{
		if (get_register_by_value(a) != NULL && get_register_by_value(b) != NULL)
			fprintf(target, "0x%04X: (skipped) %s %s %s", vm->pc, opname, get_register_by_value(a)->name, get_register_by_value(b)->name);
		else
			fprintf(target, "0x%04X: (skipped) %s 0x%04X 0x%04X", vm->pc, opname, a, b);
	}
	else
	{
		if (get_register_by_value(a) != NULL && get_register_by_value(b) != NULL)
			fprintf(target, "0x%04X: %s %s %s", vm->pc, opname, get_register_by_value(a)->name, get_register_by_value(b)->name);
		else
			fprintf(target, "0x%04X: %s 0x%04X 0x%04X", vm->pc, opname, a, b);
	}
}

void vm_print_op_nonbasic(const char* opname, vm_t* vm, uint16_t a)
{
	FILE* target = stdout;
	if (!vm->debug && vm->dump == NULL)
		return;
	if (vm->dump != NULL)
		target = vm->dump;

	if (vm->skip)
	{
		if (get_register_by_value(a) != NULL)
			fprintf(target, "0x%04X: (skipped) %s %s", vm->pc, opname, get_register_by_value(a)->name);
		else
			fprintf(target, "0x%04X: (skipped) %s 0x%04X", vm->pc, opname, a);
	}
	else
	{
		if (get_register_by_value(a) != NULL)
			fprintf(target, "0x%04X: %s %s", vm->pc, opname, get_register_by_value(a)->name);
		else
			fprintf(target, "0x%04X: %s 0x%04X", vm->pc, opname, a);
	}
}

void vm_dump_state(vm_t* vm)
{
	if (vm->dump == NULL)
		return;

	fprintf(vm->dump, "--------------------------------\n");
	fprintf(vm->dump, "A:	0x%04X	 [A]:	0x%04X\n", vm->registers[REG_A], vm->ram[vm->registers[REG_A]]);
	fprintf(vm->dump, "B:	0x%04X	 [B]:	0x%04X\n", vm->registers[REG_B], vm->ram[vm->registers[REG_B]]);
	fprintf(vm->dump, "C:	0x%04X	 [C]:	0x%04X\n", vm->registers[REG_C], vm->ram[vm->registers[REG_C]]);
	fprintf(vm->dump, "X:	0x%04X	 [X]:	0x%04X\n", vm->registers[REG_X], vm->ram[vm->registers[REG_X]]);
	fprintf(vm->dump, "Y:	0x%04X	 [Y]:	0x%04X\n", vm->registers[REG_Y], vm->ram[vm->registers[REG_Y]]);
	fprintf(vm->dump, "Z:	0x%04X	 [Z]:	0x%04X\n", vm->registers[REG_Z], vm->ram[vm->registers[REG_Z]]);
	fprintf(vm->dump, "I:	0x%04X	 [I]:	0x%04X\n", vm->registers[REG_I], vm->ram[vm->registers[REG_I]]);
	fprintf(vm->dump, "J:	0x%04X	 [J]:	0x%04X\n", vm->registers[REG_J], vm->ram[vm->registers[REG_J]]);
	fprintf(vm->dump, "PC:	0x%04X	 SP:	0x%04X\n", vm->pc, vm->sp);
	fprintf(vm->dump, "EX:	0x%04X	 IA:	0x%04X\n", vm->ex, vm->ia);
	if (vm->queue_interrupts)
		fprintf(vm->dump, "IRQ ENABLED:		  true\n");
	else
		fprintf(vm->dump, "IRQ ENABLED:		 false\n");
	fprintf(vm->dump, "IRQ COUNT:		0x%04X\n", vm->irq_count);
	fprintf(vm->dump, "CYCLES TO SLEEP:	0x%04X\n", vm->sleep_cycles);
	fprintf(vm->dump, "\n");
}

void vm_cycle(vm_t* vm)
{
	static struct timeval t;
	static long int usec = 0;

	uint16_t instruction;
	uint16_t op;
	uint16_t b;
	uint16_t a;

	if (vm->sleep_cycles > 0)
	{
		vm->sleep_cycles--;
		return;
	}

	gettimeofday(&t, NULL);

	if((t.tv_usec - usec < 0) ? usec - t.tv_usec  > 
				   16000 : t.tv_usec - usec > 16000)
	{
		usec = t.tv_usec;
		vm_hook_fire(vm, 0, HOOK_ON_60HZ);
	}

	if (!vm->queue_interrupts && vm->irq_count > 0)
	{
		a = vm->irq[0];
		printd(LEVEL_DEBUG, "sending interrupt handler with msgid %u\n");
		memcpy(&vm->irq, &vm->irq[1], INTERRUPT_MAX * sizeof(uint16_t));
		vm->irq[INTERRUPT_MAX] = 0;
		vm->irq_count--;
		vm_interrupt(vm, a);
	}

	vm_hook_fire(vm, 0, HOOK_ON_PRE_CYCLE);
	if (vm->halted) return; // skip if the precycle hook halted the virtual machine

	instruction = vm_consume_word(vm);
	op = INSTRUCTION_GET_OP(instruction);
	b = INSTRUCTION_GET_B(instruction);
	a = INSTRUCTION_GET_A(instruction);

	switch (op)
	{
		case OP_SET:
			vm_print_op("SET", vm, b, a);
			vm_op_set(vm, b, a);
			break;

		case OP_ADD:
			vm_print_op("ADD", vm, b, a);
			vm_op_add(vm, b, a);
			break;

		case OP_SUB:
			vm_print_op("SUB", vm, b, a);
			vm_op_sub(vm, b, a);
			break;

		case OP_MUL:
			vm_print_op("MUL", vm, b, a);
			vm_op_mul(vm, b, a);
			break;

		case OP_MLI:
			vm_print_op("MLI", vm, b, a);
			vm_op_mli(vm, b, a);
			break;

		case OP_DIV:
			vm_print_op("DIV", vm, a, b);
			vm_op_div(vm, b, a);
			break;

		case OP_DVI:
			vm_print_op("DVI", vm, b, a);
			vm_op_dvi(vm, b, a);
			break;

		case OP_MOD:
			vm_print_op("MOD", vm, b, a);
			vm_op_mod(vm, b, a);
			break;

		case OP_MDI:
			vm_print_op("MDI", vm, b, a);
			vm_op_mdi(vm, b, a);
			break;

		case OP_AND:
			vm_print_op("AND", vm, b, a);
			vm_op_and(vm, b, a);
			break;

		case OP_BOR:
			vm_print_op("BOR", vm, b, a);
			vm_op_bor(vm, b, a);
			break;

		case OP_XOR:
			vm_print_op("XOR", vm, b, a);
			vm_op_xor(vm, b, a);
			break;

		case OP_SHR:
			vm_print_op("SHR", vm, b, a);
			vm_op_shr(vm, b, a);
			break;

		case OP_ASR:
			vm_print_op("ASR", vm, b, a);
			vm_op_asr(vm, b, a);
			break;

		case OP_SHL:
			vm_print_op("SHL", vm, b, a);
			vm_op_shl(vm, b, a);
			break;

		case OP_IFB:
			vm_print_op("IFB", vm, b, a);
			vm_op_ifb(vm, b, a);
			break;

		case OP_IFC:
			vm_print_op("IFC", vm, b, a);
			vm_op_ifc(vm, b, a);
			break;

		case OP_IFE:
			vm_print_op("IFE", vm, b, a);
			vm_op_ife(vm, b, a);
			break;

		case OP_IFN:
			vm_print_op("IFN", vm, b, a);
			vm_op_ifn(vm, b, a);
			break;

		case OP_IFG:
			vm_print_op("IFG", vm, b, a);
			vm_op_ifg(vm, b, a);
			break;

		case OP_IFA:
			vm_print_op("IFA", vm, b, a);
			vm_op_ifa(vm, b, a);
			break;

		case OP_IFL:
			vm_print_op("IFL", vm, b, a);
			vm_op_ifl(vm, b, a);
			break;

		case OP_IFU:
			vm_print_op("IFU", vm, b, a);
			vm_op_ifu(vm, b, a);
			break;

		case OP_ADX:
			vm_print_op("ADX", vm, b, a);
			vm_op_adx(vm, b, a);
			break;

		case OP_SBX:
			vm_print_op("SBX", vm, b, a);
			vm_op_sbx(vm, b, a);
			break;

		case OP_STI:
			vm_print_op("STI", vm, b, a);
			vm_op_sti(vm, b, a);
			break;

		case OP_STD:
			vm_print_op("STD", vm, b, a);
			vm_op_std(vm, b, a);
			break;

		case OP_NONBASIC:
			switch (b)
			{
				case NBOP_JSR:
					vm_print_op_nonbasic("JSR", vm, a);
					vm_op_jsr(vm, a);
					break;

				case NBOP_INT:
					vm_print_op_nonbasic("INT", vm, a);
					vm_op_int(vm, a);
					break;

				case NBOP_IAG:
					vm_print_op_nonbasic("IAG", vm, a);
					vm_op_iag(vm, a);
					break;

				case NBOP_IAS:
					vm_print_op_nonbasic("IAS", vm, a);
					vm_op_ias(vm, a);
					break;

				case NBOP_RFI:
					vm_print_op_nonbasic("RFI", vm, a);
					vm_op_rfi(vm, a);
					break;

				case NBOP_IAQ:
					vm_print_op_nonbasic("IAQ", vm, a);
					vm_op_iaq(vm, a);
					break;

				case NBOP_HWN:
					vm_print_op_nonbasic("HWN", vm, a);
					vm_op_hwn(vm, a);
					break;

				case NBOP_HWQ:
					vm_print_op_nonbasic("HWQ", vm, a);
					vm_op_hwq(vm, a);
					break;

				case NBOP_HWI:
					vm_print_op_nonbasic("HWI", vm, a);
					vm_op_hwi(vm, a);
					break;

				default:
					vm_halt(vm, "Invalid non-basic opcode %u. (0x%04X at 0x%04X)\n", b, instruction, vm->pc);
					break;
			}

			break;

		default:
			vm_halt(vm, "Invalid opcode %u. (0x%04X at 0x%04X)\n", op, instruction, vm->pc);
			break;
	}

	if (vm->debug || vm->dump != NULL)
		fprintf(vm->dump == NULL ? stdout : vm->dump, "\n");

	if (vm->dump != NULL)
		vm_dump_state(vm);

	vm_hook_fire(vm, 0, HOOK_ON_POST_CYCLE);
}
