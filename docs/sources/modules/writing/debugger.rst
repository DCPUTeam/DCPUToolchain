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
        command `name`.  Commands are registered in a
        case-insensitive manner.
        
        .. py:function:: handler(state, params)
            :noindex:
        
            Handles a debugger command.  `state` is a table
            representing :py:class:`state`.  `params` is an array of
            tables representing :py:class:`param`.
    
    .. py:function:: add_hook(type, handler)
        :noindex:

        Registers the specified `handler` to fire every time the
        `type` event fires.  Built-in events are "precycle",
        "postcycle", "write", "break", "step", "next", "run",
        "continue" and "interrupt".  Hooks are registered in a
        case-insensitive manner.  You can fire events using the 
        `raise` method of the state object that is
        passed to handlers.
        
        .. note::
            At this time you can only have one function registered
            as a hook for given event (the same way you can only have
            one function registered for handling a given command).
        
        .. py:function:: handler(state, pos)
            :noindex:
        
            Handles a debugger hook.  `state` is a table
            representing :py:class:`state`.  `pos` is a number
            which indicates a memory position related to the
            event.

            For the `write` event, `pos` contains the address in
            RAM that was written to.

            For the `interrupt` event, `pos` contains the old PC
            value (the interrupt has already been set up when the
            event fires).

            When custom Lua modules use the `raise` method in the
            `state` object, they may pass a position parameter
            which can be used to store any numeric data.

            For all other events, `pos` will be 0.
    
    .. py:function:: add_symbol_hook(handler)
        :noindex:

        Registers the specified `handler` to fire when symbols are hit
        in the natural course of execution.
        
        .. note::
            At this time you can only have one function registered
            for handling symbols (the same way you can only have
            one function registered for handling a given command).
        
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
    
    .. py:function:: _break([code])
        :noindex:
    
        Breaks execution of the virtual machine (if it was running)
        and drops the debugger back to accepting commands.
        
        Optionally accepts a numeric return code for usage when the
        debugger is running in single command (-c) mode.  The return code
        of the debugger will be the code provided from the last `_break`
        call that actually provides a code parameter.  Calling `_break`
        without the code parameter will not affect any previously set
        return code.
        
        .. note::
            This function is named "_break" and not "break" due to
            "break" being a reserved word in the Lua lexer.
            
    .. py:function:: run()
        :noindex:
    
        Starts or continues execution of the virtual machine after
        the Lua module returns control back to the debugger (i.e. after
        the current hook or command exits).
        
    .. py:function:: symbols()
        :noindex:
    
        Returns an indexed table of entries that designate string symbols
        currently loaded.  Each entry is a table with fields "address" and
        "data" where the address indicates the memory position the symbol
        points to and "data" is the string symbol.
        
    .. py:function:: lines()
        :noindex:
    
        Returns an table of entries of line number symbols.  Operates similar
        to symbols() except that the returned table has fields "file", "line"
        and "address".  "file" is a string value and "line" is a numeric value.
        
    .. py:function:: raise(event[, pos])
        :noindex:
    
        Raises an event which can be handled by other Lua modules via the
        `add_hook` function.  The event name is the "type" of hook and the
        position parameter is passed to the hook handler in other modules.
        
Module Example
-------------------

This example evaluates assertion symbols and stops the debugger
if an assertion is not true.

.. code-block:: lua

    function assert_handler(state, symbol)
      function resolve_label(needed)
        if (state.cpu.registers[needed] == nil) then
          state:_break()
          error("unable to resolve '" .. needed .. "' for assertion evaluation (halted vm)")
        else
          return state.cpu.registers[needed]
        end
      end
      -- check to see if it's our kind of symbol.
      if (string.sub(symbol, 0, #"assertion:") == "assertion:") then
        -- handle assertion
        local expr = expression_create(string.sub(symbol, #"assertion:" + 1))
        if (expr:evaluate(resolve_label) ~= 1) then
          -- attempt to find line information
          local lineinfo = ""
          for i, v in ipairs(state:lines()) do
            if (v.address < state.cpu.registers.pc) then 
              lineinfo = " after '" .. v.file .. ":" .. v.line .. "'"
            end
          end
          -- assertion failed, break
          print("assertion \"" .. string.sub(symbol, #"assertion:" + 1) .. "\" failed" .. lineinfo .. ".")
          state:_break()
          state:raise("assertion_failed")
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
