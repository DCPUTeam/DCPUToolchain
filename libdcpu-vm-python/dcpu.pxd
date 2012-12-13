from libc.stdint cimport uint16_t, uint8_t

cdef extern from "dcpu.h":
	ctypedef struct vm_t:
		uint16_t registers[8]
		uint16_t pc
		uint16_t sp
		uint16_t ia
		uint16_t ex
		uint16_t ram[0x10000]
		uint8_t halted

	vm_t* vm_create()
	void vm_reset(vm_t* vm, bint init_memory)
	void vm_flash(vm_t* vm, uint16_t memory[0x10000])
	void vm_execute(vm_t* vm, char* execution_dump)
	void vm_free(vm_t* vm)
	void vm_cycle(vm_t* vm)

	cdef enum:
		REG_A = 0x00
		REG_B = 0x01
		REG_C = 0x02
		REG_X = 0x03
		REG_Y = 0x04
		REG_Z = 0x05
		REG_I = 0x06
		REG_J = 0x07

