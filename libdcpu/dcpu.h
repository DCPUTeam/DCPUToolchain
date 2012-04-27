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
#define REG_A     0x00
#define REG_B     0x01
#define REG_C     0x02
#define REG_X     0x03
#define REG_Y     0x04
#define REG_Z     0x05
#define REG_I     0x06
#define REG_J     0x07
#define VAL_A     0x08
#define VAL_B     0x09
#define VAL_C     0x0A
#define VAL_X     0x0B
#define VAL_Y     0x0C
#define VAL_Z     0x0D
#define VAL_I     0x0E
#define VAL_J     0x0F
#define NXT_VAL_A 0x10
#define NXT_VAL_B 0x11
#define NXT_VAL_C 0x12
#define NXT_VAL_X 0x13
#define NXT_VAL_Y 0x14
#define NXT_VAL_Z 0x15
#define NXT_VAL_I 0x16
#define NXT_VAL_J 0x17
#define PUSH_POP  0x18
#define PEEK      0x19
#define PICK      0x1A
#define SP        0x1B
#define PC        0x1C
#define EX        0x1D
#define NXT       0x1E
#define NXT_LIT   0x1F
// 0x20 -> 0x3f are all literal values.
#define IA        0xFF /* not a valid value, but is used to pass IA around internally */

// Position information
#define POS__     0x0
#define POS_A     0x1
#define POS_B     0x2

// Opcodes
#define OP_NONBASIC 0x00
#define OP_SET      0x01
#define OP_ADD      0x02
#define OP_SUB      0x03
#define OP_MUL      0x04
#define OP_MLI      0x05
#define OP_DIV      0x06
#define OP_DVI      0x07
#define OP_MOD      0x08
#define OP_MDI      0x09
#define OP_AND		0x0A
#define OP_BOR      0x0B
#define OP_XOR      0x0C
#define OP_SHR      0x0D
#define OP_ASR      0x0E
#define OP_SHL      0x0F

#define OP_IFB      0x10
#define OP_IFC      0x11
#define OP_IFE      0x12
#define OP_IFN      0x13
#define OP_IFG      0x14
#define OP_IFA      0x15
#define OP_IFL      0x16
#define OP_IFU      0x17

#define OP_ADX      0x1A
#define OP_SBX      0x1B

#define OP_STI		0x1e
#define OP_STD		0x1f

// Non-basic opcodes
#define NBOP_RESERVED   0x00
#define NBOP_JSR        0x01
#define NBOP_HCF		0xFFFF /* secret! */
#define NBOP_INT        0x08
#define NBOP_IAG        0x09
#define NBOP_IAS        0x0A
#define NBOP_RFI		0x0B
#define NBOP_IAQ		0xC
#define NBOP_HWN        0x10
#define NBOP_HWQ        0x11
#define NBOP_HWI        0x12

// Arithmetic constants
#define AR_NOFLOW       0x0000
#define AR_OVERFLOW     0x0001
#define AR_UNDERFLOW    0xffff
#define AR_MAX          0xffff

// Interrupt messages
#define INT_TIMER       0x0

// Instruction mechanisms
#define INSTRUCTION_CREATE(op, b, a) (((a & 0x3f) << 10) + ((b & 0x1f) << 5) + (op & 0x1f))
#define INSTRUCTION_GET_A(inst) ((inst & 0xfc00) >> 10)
#define INSTRUCTION_GET_B(inst) ((inst & 0x3e0) >> 5)
#define INSTRUCTION_GET_OP(inst) (inst & 0x1f)

typedef struct
{
	uint16_t registers[8];
	uint16_t pc;
	uint16_t sp;
	uint16_t ia;
	uint16_t ex;
	uint16_t ram[0x10000];
	uint16_t dummy; // Dummy position for assignments that silently fail.
	uint16_t irq[0x10000];
	uint8_t sleep_cycles;
	uint8_t queue_interrupts;
	uint8_t halted;
	uint8_t skip;
	uint8_t debug;
} vm_t;

typedef void (*hw_interrupt)(vm_t* vm);

typedef struct
{
	uint16_t id_1;
	uint16_t id_2;
	uint16_t c;
	uint16_t x;
	uint16_t y;
	hw_interrupt handler;
} hw_t;

vm_t* vm_create();
void vm_flash(vm_t* vm, uint16_t memory[0x10000]);
void vm_execute(vm_t* vm);
void vm_free(vm_t* vm);

#endif
