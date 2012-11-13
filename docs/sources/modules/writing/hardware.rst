.. _modules-hardware:

Hardware Modules
=============================================

The module block definition for hardware modules is as follows:

.. code-block:: lua

    MODULE = {
        Type = "Hardware",
        Name = "My Module",
        Version = "1.0"
    };
    
    
Module API
------------------
    
Function Hooks
~~~~~~~~~~~~~~~~~~~~

.. py:function:: interrupt(cpu)
    :noindex:

    Called when the hardware receives an interrupt from the CPU.
    `cpu` is a table matching :py:class:`cpu`.
    
.. py:function:: cycle(cpu)
    :noindex:

    Called on every CPU cycle.  Use with care as this can slow
    down emulation if it is used by a large number of custom
    hardware modules.  `cpu` is a table matching :py:class:`cpu`.
    
.. py:function:: write(cpu, pos)
    :noindex:

    Called immediately after RAM is written to, even if the
    instruction that wrote to RAM has not yet concluded.  `cpu` is
    a table matching :py:class:`cpu`.  `pos` is the memory address
    of RAM that was written to.
     
Table Hooks
~~~~~~~~~~~~~~~~~~~~~

.. py:data:: HARDWARE
    :noindex:

    A table defined as follows, used to provide information
    about the hardware device.
    
    .. code-block:: lua

        HARDWARE = {
            ID = 0x739df773,
            Version = 0x0001,
            Manufacturer = 0x93765997
        };
    
    .. note::
    
        Don't just copy the values above as it will conflict
        with existing hardware devices.
       
Module Example
-------------------

This example is a basic random number generator.

.. warning::

    Remove the :py:func:`cycle` and :py:func:`write` methods
    if you are not using them.

.. code-block:: lua

    -- interrupt values
    local INT_GENERATE = 0;
    local INT_SEED = 1;

    function interrupt(cpu)
        -- cpu is a table that lets you do things to the CPU.
        if (cpu.registers.A == INT_GENERATE) then
            cpu.registers.B = math.random(0x0, 0xFFFF);
        elseif (cpu.registers.A == INT_SEED) then
            math.randomseed(cpu.registers.B);
        end
    end

    function cycle(cpu)
        -- cpu is a table that lets you do things to the CPU.
    end

    function write(cpu, pos)
        -- cpu is a table that lets you do things to the CPU.
        -- pos is the memory address that was written to.
    end

    MODULE = {
        Type = "Hardware",
        Name = "RNG Hardware",
        Version = "1.0"
    };

    HARDWARE = {
        ID = 0x739df773,
        Version = 0x0001,
        Manufacturer = 0x93765997
    };