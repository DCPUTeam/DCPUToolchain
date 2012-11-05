.. _debugger-commands:

Available commands
======================

The debugger has a wide-range of commands available to inspect and run DCPU-16 code, including
commands that a provided by the module infrastructure (see :ref:`modules-debugger`).

load
----------------------

**Syntax:** load [symbols] file

Loads either a DCPU-16 program into memory, or loads a set of symbols.  `symbols` is a constant, not a path to
the symbol file and thus you must run `load` twice in order to load both a program and it's symbols.

.. note::

    You can also load a program and it's symbols automatically using the command line options.
    
run / continue
----------------

**Syntax:** run

**Syntax:** continue

Starts or continues execution of the DCPU-16 program.

break
-----------------

**Syntax:** break <command> <path>:<index>

*Example:* break add memory:0xbeef

*Example:* break add test.c:23
    
Manipulates breakpoints.  You can add and delete breakpoints with this command, as well as list all
currently active breakpoints.

.. note::
    
    `test.c:23` will only work correctly if you have previously loaded symbols with the `load symbols` command.

step
----------------

**Syntax:** step

Performs a single instruction of the DCPU-16.

next
----------------

**Syntax:** next

Perform a single instruction of the DCPU-16, but steps over JSR and other PC modifications such that the next
instruction linearly in memory is where execution will stop.

inspect
---------------

**Syntax:** inspect cpu

**Syntax:** inspect symbols

**Syntax:** inspect memory [<start>] [<end>]

Inspects the DCPU-16, the loaded symbols or the DCPU-16's memory.

disasm
-------------

**Syntax:** disasm [<start>] [<end>]

Disassemble a range of DCPU-16 memory.  If `start` and `end` are omitted, disassembles the code surrounding the current
instruction.  If `end` is omitted, disassembles until the end of the loaded file.

attach / detach
------------------

**Syntax:** attach hardware [<name>]

**Syntax:** detach hardware [<name>]

Attach a hardware device called `name`.  Valid hardware names are `lem1802`, `keyboard`, `clock`, `m35fd` and `sped3`.  Lua
hardware can not yet be loaded.