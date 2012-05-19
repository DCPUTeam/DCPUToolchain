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