from libc.stdint cimport uint16_t

cdef extern from "dcpu.h":
	ctypedef struct vm_t:
		uint16_t registers[8]
		uint16_t pc
		uint16_t sp
		uint16_t ia
		uint16_t ex
		uint16_t ram[0x10000]

	vm_t* vm_create()
	void vm_init(vm_t* vm, bint init_memory)
	void vm_flash(vm_t* vm, uint16_t memory[0x10000])
	void vm_execute(vm_t* vm, char* execution_dump)
	void vm_free(vm_t* vm)


