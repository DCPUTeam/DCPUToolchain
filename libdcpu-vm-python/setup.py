from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [
	Extension("dcpu", ["dcpu.pyx"],
	libraries=["libdcpu-vm-shared"])
]

setup(
	name = 'DCPU Toolchain bindings',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)
