.. _module-assert:

Runtime Assertions
=============================================

This preprocessor and debugger module pair provide runtime
assertion checking.

.ASSERT "expr"
-----------------------
This preprocessor directive informs the debugger that at runtime,
when this symbol is encountered, the expression should be evaluated.

If the expression is false, the debugger stops current execution
and displays the assertion that failed.

.. note::
    
    The expression must be contained within a string; you can't
    specify it directly.