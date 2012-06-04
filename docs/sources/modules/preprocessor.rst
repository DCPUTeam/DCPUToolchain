.. _modules-preprocessor:

Preprocessor Modules
=============================================

The module block definition for preprocessor modules is as follows:

.. code-block:: lua

    MODULE = {
        Type = "Preprocessor",
        Name = "My Module",
        Version = "1.0"
    };
    
Module API
------------------
    
Function Hooks
~~~~~~~~~~~~~~~~~~~~

.. py:function:: setup()
    :noindex:

    Called when the preprocessor initializes the module and is
    requesting that appropriate directives be registered.
    
    The following functions are available during the :py:func:`setup`
    call:
    
    .. py:function:: add_preprocessor_directive(name, handler)
        :noindex:

        Registers the specified `handler` function with the specified
        directive `name`.  Directives are registered in a
        case-insensitive manner.
        
        .. py:function:: handler(state, params)
            :noindex:
        
            Handles a preprocessor directive.  `state` is a table
            representing :py:class:`state`.  `params` is an array of
            tables representing :py:class:`param`.
            
.. py:class:: state
    :noindex:

    .. py:function:: print(text)
        :noindex:
    
        Outputs `text` to the result, *without* a newline.
        
    .. py:function:: print_line(text)
        :noindex:
    
        Outputs `text` to the result, *with* a newline.  If `text`
        is an empty string or nil, outputs just a newline.
        
    .. py:function:: scope_enter(output)
        :noindex:
    
        Enters a new scope, with `output` a boolean value indicating
        whether or not text in this scope should be outputted in the
        result.

    .. py:function:: scope_exit()
        :noindex:
    
        Exits the current scope.
        
    .. py:function:: add_symbol(symbol)
        :noindex:
    
        Adds a string-based symbol at this location which will be
        available during debugging modules.  See also
        :ref:`modules-debugger`.
    
.. py:class:: param
    :noindex:

    .. py:attribute:: type
        :noindex:
    
        The type of the parameter.
        
        Under the debugger, will be either "PATH", "PARAM",
        "NUMBER" or "STRING".
        
        Under the preprocessor, will be either "WORD", "STRING"
        "EXPRESSION" or "NUMBER".
        
        See also :py:class:`expression`.
    
    .. py:attribute:: value
        :noindex:
    
        The value of the parameter, either a string, number or,
        expression object depending on the type of the parameter.

        
Module Example
-------------------

This example outputs assertion symbols that are checked when the
assertion debugging module is loaded.

.. code-block:: lua

    function assert_handler(state, params)
      -- we expect a single parameter that is an expression.
      if (#params ~= 1 or params[1].type ~= "STRING") then
        error("error: .ASSERT directive expects single expression parameter.")
      end
      local expr = expression_create(params[1].value);

      -- output a symbol for the expression.
      state:add_symbol("assertion:" .. expr:representation());
    end

    function setup()
      -- perform setup
      add_preprocessor_directive("ASSERT", assert_handler, false, true)
    end

    MODULE = {
      Type = "Preprocessor",
      Name = ".ASSERT directive",
      Version = "1.0"
    };
