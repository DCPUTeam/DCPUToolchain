.. highlightlang:: none

.. _asm-preprocessor-syntax:

Assembler Syntax
=============================================

The following syntax is applicable to the assembler preprocessor.

.. _asm-preprocessor-directives:

Directives
---------------

In all preprocessor directives, the leading character can either be
`.` or `#`.  The preprocessor directives are case insensitive.
The preprocessor provides the following directives:

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

    Include the specified file relative to the current file being preprocessed.
    
.. ppdirective:: .INCLUDE <filename>

    Include the specified file by searching through the include directories.

.. ppdirective:: .EQUATE name value
                  .EQU name value
                  .DEFINE name value
    
    Defines the specified preprocessor constant with name `name` to
    evaluate to the specified value `value`.  `value` is from the first
    character to the end of the line, including whitespace.  `\\` can be used
    to wrap the definition over multiple lines.
    
    When `name` is located in the source text from now on, it will be replaced with `value`.

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
    
.. ppdirective:: name(valueA, valueB)

    Evaluates the macro with the name `name`.  Each passed value is result
    of each parameter that will be transformed.
