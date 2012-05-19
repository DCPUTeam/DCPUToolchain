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
    
    .. py:function:: add_preprocessor_directive(name, handler[, flattenParams, noEvaluation])
        :noindex:

        Registers the specified `handler` function with the specified
        directive `name`.  If `flattenParams` is true, then a single
        string representing the rest of the line will be passed to
        the handler (instead of the line being parsed by the lexer).
        If `noEvaluation` is true, expression trees will be passed
        as-is instead of evaluated.
        
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
        :ref:`module-debugging`.
    
.. py:class:: param
    :noindex:

    .. py:attribute:: type
        :noindex:
    
        The type of the parameter.  Will be either "WORD", "PATH",
        "STRING", "NUMBER" or "EXPRESSION".
        
        See also :py:class:`expression`.
    
    .. py:attribute:: value
        :noindex:
    
        The value of the parameter, either a string or number,
        depending on the type of the parameter.

        
Module Example
-------------------

This example outputs assertion symbols that are checked when the
assertion debugging module is loaded.

.. code-block:: lua

    function assert_handler(state, params)
        -- we expect a single parameter that is an expression.
        if (#params ~= 1 or params[0].type ~= "EXPRESSION")
            print("error: .ASSERT directive expects single expression parameter.")
        end
        
        -- output a symbol for the expression.
        state.add_symbol("assertion:" .. params[0].representation());
    end

    function setup()
        -- perform setup
        add_preprocessor_directive("ASSERT", assert_handler, false, true)
    end

    MODULE = {
        Type = "Preprocessor",
        Name = ".ASSERT Directive",
        Version = "1.0"
    };