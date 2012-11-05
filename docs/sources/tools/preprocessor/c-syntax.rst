.. highlightlang:: none

.. _c-preprocessor-syntax:

C Syntax
=============================================

The following syntax is applicable to the C preprocessor.

.. _c-preprocessor-directives:

Directives
---------------

The leading character for C preprocessor directives must be `#`.  All
directives must be in lowercase.  The C preprocessor provides the
following directives:

.. ppdirective:: #line number "filename"

    Sets the current line number and filename to the specified values,
    passing this information into the underlying processor so that errors
    will be reported relative to these addresses.  The specified addresses
    are also used to determine the location of debugging symbols.

.. ppdirective:: #include "filename"

    Include the specified file relative to the current file being preprocessed.
    
.. ppdirective:: #include <filename>

    Include the specified file by searching through the include directories.

.. ppdirective:: #define name value
    
    Defines the specified preprocessor constant with name `name` to
    evaluate to the specified value `value`.  `value` is from the first
    character to the end of the line, including whitespace.  `\\` can be used
    to wrap the definition over multiple lines.
    
    When `name` is located in the source text from now on, it will be replaced with `value`.

.. ppdirective:: #undef name

    Undefines the specified preprocessor constant with name `name`.

.. ppdirective:: #ifdef name

    Enters an if-defined block, where the source text inside the block
    is only outputted to the result if the preprocessor constant with
    the name `name` is defined at this point.

.. ppdirective:: #ifndef name

    Similar to :ppdir:`#ifndef`, except that the source text inside the
    block is only outputted if the specified preprocessor constant
    with the name `name` is not defined at this point.

.. ppdirective:: #else

    Specifies an alternate block of source text to be outputted only
    if the previous block of source text was not outputted.

.. ppdirective:: #endif

    Terminates a conditional block of source text that begun with
    either :ppdir:`#ifdef`, :ppdir:`#ifndef` or :ppdir:`#else`.

.. ppdirective:: #define name(paramA, paramB)

    Begins a preprocessor macro definition where the source text is
    evaluated with each parameter replaced with the contents of the
    passed value when the macro is evaluated.
    
    Unlike assembly, C preprocessor macros must use `\\` at the end
    of a line to signal a multiline macro.
    
.. ppdirective:: name(valueA, valueB)

    Evaluates the macro with the name `name`.  Each passed value is result
    of each parameter that will be transformed.
