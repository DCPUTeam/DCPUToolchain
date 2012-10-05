cimport dcpu

cdef class DCPU:
	cdef dcpu.vm_t *_vm

	def __cinit__(self):
		self._vm = vm_create()

	def __dealloc__(self):
		if self._vm is not NULL:
			dcpu.vm_free(self._vm)

	cdef lol(self):
		pass
