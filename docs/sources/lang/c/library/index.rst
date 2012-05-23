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
   
    stdlib.h.rst
    string.h.rst
    math.h.rst

.. _compiler-stdlib-c-linking:

Linking
-----------------------

When linking C applications, you should provide `stdlib.dlib16` as the first object to link against.  The
standard library provides startup mechanisms that must be run before your application starts and thus it
is essential that it is the entry point for the image.

This mechanism will replace the existing `bootstrap.asm` file as the bootstrap assembly is now contained
inside the standard library.

The other advantage of merging `bootstrap.asm` into the standard library is that it is no longer necessary
to provide the `-e` option to the compiler.

.. warning::
    
    We encourage people to move to the new standard of linking against `stdlib.dlib16` as `bootstrap.asm`
    and the `-e` compiler option will be going away soon.
