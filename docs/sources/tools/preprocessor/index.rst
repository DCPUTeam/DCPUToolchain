.. highlightlang:: none

.. _preprocessor:

Preprocessor
=============================================

The preprocessor parses and transforms text before it is read in by the compiler or assembler.  In the toolchain we
use the preprocessor for both the compiler infrastructure and the assembler, thus all directives in the preprocessor
are supported in both high-level languages and assembly.

.. _preprocessor-directives:

Directives
---------------

In all preprocessor directives, the leading character can either be
`.` (assembly standard) or `#` (C standard).  They can be used
interchangably.  The preprocessor provides the following directives:

.. ppdirective:: .LINE number "filename"

    Sets the current line number and filename to the specified values,
    passing this information into the underlying processor so that errors
    will be reported relative to these addresses.  The specified addresses
    are also used to determine the location of debugging symbols.
    
.. ppdirective:: .ULINE number "filename"

    Sets the secondary (or upper) line number and filename to the
    specified values.  This allows processors to keep track of two sets
    of positions, highly useful for the assembler where it must keep track
    of both it's own line number and filename for debugging symbols in
    addition to the line number and filename provided by the compiler.

.. ppdirective:: .INCLUDE "filename"

    Includes the specified file recursively.

.. ppdirective:: .EQUATE name "value"
                  .EQU name "value"
                  .DEFINE name "value"
    
    Defines the specified preprocessor constant with name `name` to
    evaluate to the specified value `value`.  When `name` is located
    in the source text from now on, it will be replaced with `value`.

.. ppdirective:: .UNDEF name

    Undefines the specified preprocessor constant with name `name`.

.. ppdirective:: .IFDEF name

    Enters an if-defined block, where the source text inside the block
    is only outputted to the result if the preprocessor constant with
    the name `name` is defined at this point.

.. ppdirective:: .IFNDEF name

    Similar to :ppdir:`.IFDEF`, except that the source text inside the
    block is only outputted if the specified preprocessor constant
    with the name `name` is not defined at this point.

.. ppdirective:: .ELSE

    Specifies an alternate block of source text to be outputted only
    if the previous block of source text was not outputted.

.. ppdirective:: .ENDIF

    Terminates a conditional block of source text that begun with
    either :ppdir:`.IFDEF`, :ppdir:`.IFNDEF` or :ppdir:`.ELSE`.

.. ppdirective:: .MACRO name(paramA, paramB)

    Begins a preprocessor macro definition where the source text is
    evaluated with each parameter replaced with the contents of the
    passed value when the macro is called.

.. ppdirective:: .ENDMACRO

    Signals the end of a macro definition.
    
.. ppdirective:: ..name(valueA, valueB)

    Evaluates the macro with the name `name` (the double `.` or double
    `#` at the beginning is required).  Each passed value is result
    of each parameter that will be transformed.  As with other preprocessor
    definitions, macro calls can only be placed on their own line, with
    only whitespace permitted between the start of the line and the leading
    `.` or `#`.

.. _preprocessor-expressions:

Constant Expressions
---------------------------

The preprocessor supports constant expressions comprising of literals that evaluate to numeric values
(all literals except strings evaluate to single, numeric values).  All expressions are calculated
as 16-bit unsigned integers.

The following expression operations are supported:

.. ppexpressionop:: a + b

    Adds `a` to `b`.

.. ppexpressionop:: a - b

    Subtracts `b` from `a`.
    
.. ppexpressionop:: a / b

    Divides `a` by `b`.

.. ppexpressionop:: a * b

    Multiplies `a` and `b`.

.. ppexpressionop:: a % b

    Returns the modulo of `a` over `b`.

.. ppexpressionop:: a == b

    Returns 1 if `a` is equal to `b`, 0 otherwise.

.. ppexpressionop:: a != b

    Returns 1 if `a` is not equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a < b

    Returns 1 if `a` is less than `b`, 0 otherwise.
    
.. ppexpressionop:: a <= b

    Returns 1 if `a` is less than or equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a > b

    Returns 1 if `a` is greater than `b`, 0 otherwise.
    
.. ppexpressionop:: a >= b

    Returns 1 if `a` is greater than or equal to `b`, 0 otherwise.
    
.. ppexpressionop:: a & b

    The binary and of `a` and `b`.
    
.. ppexpressionop:: a | b

    The binary or of `a` and `b`.
    
.. ppexpressionop:: a ^ b

    The exclusive or of `a` and `b`.