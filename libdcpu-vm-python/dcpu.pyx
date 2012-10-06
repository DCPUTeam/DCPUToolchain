cimport dcpu
from libc.stdint cimport uint16_t

cdef class DCPU:
	cdef dcpu.vm_t *_vm

	reg_mapping = {
		'A': dcpu.REG_A,
		'B': dcpu.REG_B,
		'C': dcpu.REG_C,
		'X': dcpu.REG_X,
		'Y': dcpu.REG_Y,
		'I': dcpu.REG_I,
		'J': dcpu.REG_J
	}
	
	def __cinit__(self):
		self._vm = vm_create()

	def __dealloc__(self):
		if self._vm is not NULL:
			dcpu.vm_free(self._vm)
	
	def getmemory(self):
		memory = []
		for i in range(0x10000):
			memory.append(self._vm.ram[i])
	
		return memory
	
	def setmemory(self, memory):
		cdef uint16_t[0x10000] c_memory
		cdef int i

		for i in range(0x10000):
			try:
				c_memory[i] = memory[i]
			except IndexError:
				c_memory[i] = 0

		dcpu.vm_flash(self._vm, c_memory)

	memory = property(getmemory, setmemory)

	def getregisters(self):
		registers = {}
		for register in self.reg_mapping:
			registers[register] = self._vm.registers[self.reg_mapping[register]]

		return registers
	
	def setregisters(self, r):
		for register in r:
			self._vm.registers[self.reg_mapping[register]] = r[register]

	# NOTE: Cython doesn't seem to like decorators, so I'm declaring the properties manually.	
	registers = property(getregisters, setregisters)
	
	def execute(self):
		dcpu.vm_execute(self._vm, NULL)
	
	def stop(self):
		self._vm.halted = 1
	
	def cycle(self):
		dcpu.vm_cycle(self._vm)
		return None
