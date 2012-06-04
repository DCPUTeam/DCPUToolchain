.. _modules-writing:

Writing Modules
=============================================

All modules, regardless of what service they extend have a constant format for defining
their type, who wrote them and the version number applicable.

All modules are written in `Lua <http://www.lua.org/>`_, a highly flexible and extendable language designed
for extending applications.

.. warning::

    When the toolchain is at a point where it is able to compile it's own preprocessor
    and assembler to the DCPU, it is not guaranteed that Lua modules will be able to
    run under this environment due to memory restrictions (despite how memory efficient
    Lua is).
    
Module Format
--------------------

Every module consists of a series of user-defined functions, hooking functions and tables (specified by
the module type) and a general module information table.  Thus every module will at least look like:

.. code-block:: lua

    -- Code relevant to the type of module you are writing
    -- will go here.

    MODULE = {
        Type = "Type", 
        Name = "My Module",
        Version = "1.0"
    };

Each type of module is addressed in it's own documentation section.  The following module types
exist:

  * :ref:`modules-hardware`
  * :ref:`modules-preprocessor`
  * :ref:`modules-debugger`

Module Location
--------------------

All modules are placed in the `modules/` folder.  The type of module is used by each tool to determine
whether or not the module will be loaded and used.

Alternatively, if the :envvar:`TOOLCHAIN_MODULES` is set in the current environment, modules will be loaded
from that path *instead*.

.. envvar:: TOOLCHAIN_MODULES

    The path to load modules from instead of the `modules/` folder.

Global APIs
-------------------

These APIs apply to all types of modules, although the specific internal semantics may vary
between each tool.

Expressions
~~~~~~~~~~~~~~~~~~

.. py:function:: expression_create(str)

    Parses `str` into an expression, returning a table that
    matches :py:class:`expression`, or nil if the string could
    not be parsed successfully.

.. py:class:: expression

    .. py:attribute:: a
        :noindex:
    
        The first operand.
        
    .. py:attribute:: b
        :noindex:
    
        The second operand.
        
    .. py:attribute:: a_type
        :noindex:
    
        The type of the first operand, either "EXPRESSION" or "NUMBER".
        
    .. py:attribute:: b_type
        :noindex:
    
        The type of the second operand.
        
    .. py:attribute:: op
        :noindex:
    
        The expression operator, either nil (invalid), "+",
        "-", "*", "/", "%", "==", "!=", "<", "<=", ">",
        ">=", "&", "|" or "^".
        
        See also :ref:`preprocessor-expressions`.

    .. py:function:: evaluate()
        :noindex:
    
        Evaluates the expression, returning the numeric result.
        
    .. py:function:: representation()
        :noindex:
    
        Returns the string representation of this expression,
        recursively representing all sub-expressions inside this
        expression.
        
Virtual Machine
~~~~~~~~~~~~~~~~~~~~~~

.. py:class:: cpu

    .. py:attribute:: ram
        :noindex:
    
        An array of 0x10000 elements through which the values
        of individual RAM addresses can be retrieved and modified.
        
        .. code-block:: lua

            a = cpu.ram[0x0100];    -- Get the value at 0x0100.
            cpu.ram[0x200] = a * 2; -- Double it and store in 0x0200.
            
    .. py:attribute:: registers
        :noindex:
    
        An object through which CPU registers can be retrieved and
        modified.  The valid CPU registers are `A`, `B`, `C`,
        `X`, `Y`, `Z`, `I`, `J`, `PC`, `SP`, `EX`, `IA`
        (case-insensitive).
        
        .. code-block:: lua

            val = cpu.registers.A;     -- Get the value in register A.
            cpu.registers.PC = a * 2;  -- Jump current execution to double the value of A.
            cpu.registers.EX = 0xFFFF; -- Set the overflow register to 0xFFFF.
    
    .. py:function:: disassemble(address)
        :noindex:
    
        Returns an :py:class:`instruction` object that represents the
        instruction located at the address `address`.

.. py:class:: instruction

    A table representing a DCPU-16 instruction.

    .. py:attribute:: original
        :noindex:
        
        A table will the following fields:
        
        .. py:attribute:: full
            :noindex:
            
            The full, original 16-bit word.
        
        .. py:attribute:: op
            :noindex:
            
            The original opcode, before resolving into non-basic opcodes.
            
        .. py:attribute:: a
            :noindex:
            
            The original A parameter, before resolving into a value.
            
        .. py:attribute:: b
            :noindex:
            
            The original B parameter, before resolving into a value.
            
    .. py:attribute:: pretty
        :noindex:
        
        A table will the following fields:
        
        .. py:attribute:: op
            :noindex:
            
            The pretty printed, string representation of the opcode.  Can
            be nil if the value at the disassembled address does not
            represent any known instruction.
            
        .. py:attribute:: a
            :noindex:
            
            The pretty printed, string representation of the A parameter.
            Should only be nil if `op` is also nil.
            
        .. py:attribute:: b
            :noindex:
            
            The pretty printed, string representation of the B parameter.
            Can be nil if `op` is either nil or if the instruction is non-basic.
            
    .. py:attribute:: op
        :noindex:
        
        The resolved opcode, taking into account non-basic resolution.  Thus
        this field can contain both basic and non-basic opcodes (use `original.op`
        to determine what this field contains).
            
    .. py:attribute:: a
        :noindex:
        
        The resolved A value.  The A parameter is evaluated in order to fill this
        field and thus this represents the value of a memory address, register, etc.
        
    .. py:attribute:: b
        :noindex:
        
        The resolved B value.  The B parameter is evaluated in order to fill this
        field and thus this represents the value of a memory address, register, etc.
        If the instruction is non-basic, this field will be 0 (not nil!).
        
    .. py:attribute:: size
        :noindex:
        
        The total number of words that this instruction occupies.  This value will
        either be 1, 2 or 3 (unless the DCPU-16 specification changes in the future).
        
    .. py:attribute:: extra
        :noindex:
        
        An array of the additional words (excluding the first word which can be read
        using `original.full` field) used by this instruction.  This array does not
        sort the values; they are in the order that they are in memory.  Thus this
        array could contain a single word that matches up to either the A or B parameter.
        This array at most contains 2 entries (at indicies 1 and 2).
        
    .. py:attribute:: next
        :noindex:
        
        An array of the next values used by this instruction.  Unlike `extra`, this
        array is mapped such that the value at `next[1]` represents the next value for
        the A parameter and the value at `next[2]` represents the next value for the
        B parameter.  If either parameter does not use a next value, then the associated
        value in this array will be nil.
        