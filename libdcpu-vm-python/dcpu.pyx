cimport dcpu
from libc.stdint cimport uint16_t

class RegisterException(Exception):
	pass

class MemoryException(Exception):
	pass

cdef class DCPUMemory:
	cdef dcpu.vm_t *_vm

	def __getitem__(self, key):
		if isinstance(key, slice):
			indices = key.indices(0x10000)
			return [self[i] for i in range(*indices)]
			
		if key > 0x10000 or key < 0:
			raise IndexError

		return self._vm.ram[key]
	
	def __setitem__(self, key, value):
		if isinstance(key, slice):
			indices = key.indices(0x10000)
			for i in range(*indices):
				self[i] = value[i]
		else:
			if key > 0x10000 or key < 0:
				raise IndexError
			
			self._vm.ram[key] = value

	def __iter__(self):
		for i in range(0x10000):
			yield self._vm.ram[i]

cdef class DCPURegisters:
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

	def __getitem__(self, key):
		try:
			register = self.reg_mapping[key]
		except KeyError:
			raise RegisterException("There is no register by that name")

		return self._vm.registers[register]
	
	def __setitem__(self, key, value):
		try:
			register = self.reg_mapping[key]
		except KeyError:
			raise RegisterException("There is no register by that name")

		self._vm.registers[register] = value
	
	def __repr__(self):
		d = {}

		for register in self.reg_mapping:
			d[register] = self[register]

		return repr(d)
	
cdef make_memory(vm_t* vm):
	obj = DCPUMemory()
	obj._vm = vm

	return obj 

cdef make_registers(vm_t* vm):
	obj = DCPURegisters()
	obj._vm = vm

	return obj

cdef class DCPU:
	cdef dcpu.vm_t *_vm
	cdef readonly object memory
	cdef readonly object registers

	def __cinit__(self):
		self._vm = dcpu.vm_create()
		self.memory = make_memory(self._vm)
		self.registers = make_registers(self._vm)

	def __dealloc__(self):
		if self._vm is not NULL:
			dcpu.vm_free(self._vm)

	def execute(self):
		dcpu.vm_execute(self._vm, NULL)
	
	def stop(self):
		self._vm.halted = 1
	
	def cycle(self):
		dcpu.vm_cycle(self._vm)

