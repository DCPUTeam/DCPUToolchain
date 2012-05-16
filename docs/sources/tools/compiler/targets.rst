.. highlightlang:: none

.. _compiler-targets:

Assembler Targets
=======================

Different assemblers support different directives and features.  The toolchain compiler allows you to target
a specific assembler thus preventing the compiler from outputting assembly code that will not compile
under the specified target assembler.

The following assembler targets are supported (as well as the known list of features they support):

+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+
| Type                  | .BOUNDARY | .DATA sections | .DAT instruction | Reordered addresses | Character literals | .IMPORT directive | .EXPORT directive |
+=======================+===========+================+==================+=====================+====================+===================+===================+
| toolchain             | Yes       | `-`            | Yes              | Yes                 | Yes                | Yes               | Yes               |
+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+
| studio                | `-`       | `-`            | Yes              | `-`                 | `-`                | `-`               | `-`               |
+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+
| organic               | `-`       | `-`            | Yes              | Yes                 | Yes                | `-`               | `-`               |
+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+

Functionality Components
--------------------------

Each of the listed features is required to enable one-or-more components of language functionality.  Each feature
is described below:

.BOUNDARY directive
~~~~~~~~~~~~~~~~~~~~
Provides a safety boundary for debugging purposes.

.. code-block:: nasm

    :functionA
        SET A, B
        SET PUSH, C

    .BOUNDARY

    :functionB
        SET B, A
        ADD B, 5

**Affected functionality:** The compiler will fall back to using the .DAT directive to replicate this
functionality if it is not available.

.DATA section
~~~~~~~~~~~~~~~~~
Provides a contigious block of data declarations.

.. code-block:: nasm

    .DATA
        myString "something that I want to embed"
        myNumber 45
    
    .CODE
        SET A, 5

**Affected functionality:** No current effect.

DAT instruction
~~~~~~~~~~~~~~~~~
Provides inline ability to embed raw data.

.. code-block:: nasm

    SET A, 5
    SET PC, continueHere
    :someData
        DAT "abc", 'g', anotherLabel, 0x0
    :continueHere
    ADD A, 10

**Affected functionality:** The compiler will refuse to compile without this functionality as it is
impossible to embed string constants or any other non-numeric literal.

Reordered addresses
~~~~~~~~~~~~~~~~~~~~
Addresses can be used in the form [REG+ADDR] in addition to the standard form [ADDR+REG].

.. code-block:: nasm

    SET [A+0x1000], 5
    SET [0x1000+A], 10

**Affected functionality:** No current effect.  The compiler always uses the standard format.

Character literals
~~~~~~~~~~~~~~~~~~~
Embeds character literals such as 'a' directly in the assembly to make the result more
readable and understandable.

.. code-block:: nasm

    SET [0x8000], 0xF000
    ADD [0x8000], 'H'

**Affected functionality:** The compiler will translate characters to their ASCII values and use those
in the final resulting assembly instead.

.IMPORT directive
~~~~~~~~~~~~~~~~~~~
Declares that a specified label will be provided at link time.

.. code-block:: nasm

    .IMPORT labelFromAnotherFile
    .IMPORT anotherFunctionFromFile
    
    :someFunction
        JSR labelFromAnotherFile
    
    :anotherFunction
        JSR anotherFunctionFromFile

**Affected functionality:** The compiler will disable the ability to predeclare functions.

.EXPORT directive
~~~~~~~~~~~~~~~~~~~
Declares that a specified label should be exported in the link format and available
to other objects at link time.

.. code-block:: nasm

    .EXPORT labelFromAnotherFile
    .EXPORT anotherFunctionFromFile
    
    :labelFromAnotherFile
        SET PC, POP
    
    :labelFromAnotherFile
        SET PC, POP

**Affected functionality:** The compiler will not export functions into the link table for 
use at link time.
