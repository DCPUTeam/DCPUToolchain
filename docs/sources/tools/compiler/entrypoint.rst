.. highlightlang:: none

.. _compiler-entry-point:

Entry Points
---------------------

Some languages require extra helping functionality when they are compiled, most commonly
advanced stack management.

Each language has it's own bootstrap assembly code, however, when compiling multiple source
files into their own objects, the bootstrap code should only be included once.

The `-e` option indicates that the bootstrap assembly should be attached to this
specific object that is produced.  At link time, this object must be specified as the first
object file for linker inputs.

See also :ref:`linker`.
