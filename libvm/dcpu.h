/**

	File:			dcpu.h

	Project:		DCPU-16 Tools
	Component:		LibVM

	Authors:		James Rhodes

	Description:	Declares common macros and functions
					use in all DCPU-16 Tools components.

**/

// This is correct as of version 1.1 of the DCPU.

#ifndef __DCPU_H
#define __DCPU_H

#include <stdint.h>

// Boolean stuff.
#ifndef __cplusplus
#ifndef __GNUC__
typedef uint8_t bool;
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif
#endif

// Value definitions
#define REG_A 0x00
#define REG_B 0x01
#define REG_C 0x02
#define REG_X 0x03
#define REG_Y 0x04
#define REG_Z 0x05
#define REG_I 0x06
#define REG_J 0x07
#define VAL_A 0x08
#define VAL_B 0x09
#define VAL_C 0x0A
#define VAL_X 0x0B
#define VAL_Y 0x0C
#define VAL_Z 0x0D
#define VAL_I 0x0E
#define VAL_J 0x0F
#define NXT_VAL_A 0x10
#define NXT_VAL_B 0x11
#define NXT_VAL_C 0x12
#define NXT_VAL_X 0x13
#define NXT_VAL_Y 0x14
#define NXT_VAL_Z 0x15
#define NXT_VAL_I 0x16
#define NXT_VAL_J 0x17
#define POP  0x18
#define PEEK 0x19
#define PUSH 0x1A
#define SP   0x1B
#define PC   0x1C
#define O    0x1D
#define NXT  0x1E
#define NXT_LIT 0x1F
// 0x20 -> 0x3f are all literal values.

// Opcodes
#define OP_NONBASIC 0x0
#define OP_SET      0x1
#define OP_ADD      0x2
#define OP_SUB      0x3
#define OP_MUL      0x4
#define OP_DIV      0x5
#define OP_MOD      0x6
#define OP_SHL      0x7
#define OP_SHR      0x8
#define OP_AND      0x9
#define OP_BOR      0xA
#define OP_XOR      0xB
#define OP_IFE      0xC
#define OP_IFN      0xD
#define OP_IFG      0xE
#define OP_IFB      0xF

// Non-basic opcodes
#define NBOP_RESERVED   0x00
#define NBOP_JSR        0x01
// 0x02 -> 0x3f are all reserved
#define NBOP_EXT_PRINT  0x02 // Custom extension.

// Other
#define AR_NOFLOW 0x0000
#define AR_OVERFLOW 0x0001
#define AR_UNDERFLOW 0xffff
#define AR_MAX 0xffff

// Instruction mechanisms
#define INSTRUCTION_CREATE(op, a, b) (((b & 0x3f) << 10) + ((a & 0x3f) << 4) + (op & 0xf))
#define INSTRUCTION_GET_B(inst) ((inst & 0xfc00) >> 10)
#define INSTRUCTION_GET_A(inst) ((inst & 0x3f0) >> 4)
#define INSTRUCTION_GET_OP(inst) (inst & 0xf)

typedef struct
{
	uint16_t registers[8];
	uint16_t pc;
	uint16_t sp;
	uint16_t o;
	uint16_t ram[0x10000];
	uint16_t dummy; // Dummy position for assignments that silently fail.
	uint8_t halted;
	uint8_t skip;
	uint8_t debug;
} vm_t;

vm_t* vm_create();
void vm_flash(vm_t* vm, uint16_t memory[0x10000]);
void vm_execute(vm_t* vm);
void vm_free(vm_t* vm);

#endif
