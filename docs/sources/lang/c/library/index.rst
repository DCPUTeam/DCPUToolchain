.. highlightlang:: c

.. _compiler-stdlib-c:

C Standard Library
=============================================

We haven't yet finishing documenting or designing this!

What we can say is there will be two elements to the standard library:

  * The standard library
  * The compatibility layer
  
The compatibility layer will provide the same C functions that are available to applications compiled
for normal computers.  The reason that we're relegating this as a compatibility layer and instead
modelling our own standard C library is for security.  Quite a few of the standard ANSI C functions
are not secure (susceptable to buffer overflows) and quite a few POSIX functions don't make sense
on the DCPU at all.

.. _compiler-stdlib-c-modules:

Module List
-----------------------

.. toctree::
    :maxdepth: 1
   
    bstring.h.rst

.. _compiler-stdlib-c-bootstrap:

Bootstrap
-----------------------

Currently provided is a `bootstrap.asm` file.  While not a standard library in the classic sense, this
file is essential for providing stack management and halting functions for C code.

This file does not expose any usable functions to C code, all of the provided functionality is used
implicitly by the compiler.  You must use the `-e` option on the compiler to get the bootstrap
attached to the object code.

It should only be included once in a project (i.e. you should only use `-e` on a single file; your main
entry point file).  See the section on :ref:`compiler-entry-point` for more information.
