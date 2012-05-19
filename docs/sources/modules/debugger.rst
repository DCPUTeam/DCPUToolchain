.. _modules-debugger:

Debugger Modules
=============================================

The module block definition for debugger modules is as follows:

.. code-block:: lua

    MODULE = {
        Type = "Debugger",
        Name = "My Module",
        Version = "1.0"
    };
    
Module API
------------------
    
Function Hooks
~~~~~~~~~~~~~~~~~~~~

.. py:function:: setup()
    :noindex:

    Called when the debugger initializes the module and is
    requesting that appropriate commands be registered.
    
    The following functions are available during the :py:func:`setup`
    call:
    
    .. py:function:: add_command(name, handler)
        :noindex:

        Registers the specified `handler` function with the specified
        command `name`.
        
        .. py:function:: handler(state, params)
            :noindex:
        
            Handles a debugger command.  `state` is a table
            representing :py:class:`state`.  `params` is an array of
            tables representing :py:class:`param`.
    
    .. py:function:: add_hook(type, handler)
        :noindex:

        Registers the specified `handler` to fire every time the
        `type` event fires.  Supported events are "cycle" and "write".
        
        .. py:function:: handler(state)
            :noindex:
        
            Handles a debugger hook.  `state` is a table
            representing :py:class:`state`.
    
    .. py:function:: add_symbol_hook(handler)
        :noindex:

        Registers the specified `handler` to fire when symbols are hit
        in the natural course of execution.
        
        .. py:function:: handler(state, symbol)
            :noindex:
        
            Handles a debugger symbol hook.  `state` is a table
            representing :py:class:`state`.  `symbol` is the string
            symbol that was hit.
    
.. py:class:: state
    :noindex:

    .. py:attribute:: cpu
        :noindex:
    
        `cpu` is a table matching :py:class:`cpu` representing
        the current state of the virtual machine.
    
    .. py:function:: break()
        :noindex:
    
        Breaks execution of the virtual machine (if it was running)
        and drops the debugger back to accepting commands.
            
Module Example
-------------------

This example evaluates assertion symbols and stops the debugger
if an assertion is not true.

.. code-block:: lua

    function assert_handler(state, symbol)
        -- check to see if it's our kind of symbol.
        if (string.sub(symbol, 0, #"assertion:")) then
            -- handle assertion
            local expr = expression_create(string.sub(symbol, #"assertion:"))
            if (expr.evaluate() ~= 0) then
                -- assertion failed, break
                print("assertion (" .. string.sub(symbol, #"assertion:") .. ") != 0")
                state.break()
            end
        end
    end

    function setup()
        -- perform setup
        add_symbol_hook(assert_handler)
    end

    MODULE = {
        Type = "Debugger",
        Name = "Assertion Module",
        Version = "1.0"
    };