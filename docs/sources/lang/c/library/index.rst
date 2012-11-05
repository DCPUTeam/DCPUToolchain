.. highlightlang:: c

.. _compiler-stdlib-c:

C Standard Library
=============================================

This is a subset of the standard C library.

.. _compiler-stdlib-c-modules:

Module List
-----------------------

.. toctree::
    :maxdepth: 1
   
    stdio.h.rst
    stdint.h.rst
    stdlib.h.rst
    stdarg.h.rst
    string.h.rst
    math.h.rst

.. _compiler-stdlib-c-linking:

Linking
-----------------------

When linking C applications, you should provide `stdlib.dlib16` as the first object to link against.  The
standard library provides startup mechanisms that must be run before your application starts and thus it
is essential that it is the entry point for the image.

.. note::
    
    In the future, the compiler will automatically insert symbols to inform the linker of this requirement
    and thus linking against the standard library will become an automatic step.