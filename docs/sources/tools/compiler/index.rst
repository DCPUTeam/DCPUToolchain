.. highlightlang:: none

.. _compiler:

Compiler
=============================================

The compiler allows you to translate (relatively) high-level languages such as C into assembly for the DCPU.  The compile infrastructure is written in such a way that it permits any language to be defined for compilation to the DCPU.

.. toctree::
    :maxdepth: 3

.. _compiler-command-line:

Command line
---------------

When invoking the compiler, you may specify any of these options

    `dtcc [-eh] -o output [-t type] input`

The most common use case is compiling a C file into assembler:

    `dtcc -o test.dasm16 test.c`

.. _compiler-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h

    Shows a list of all currently supported command-line options.

.. cmdoption:: -o output
            --output=output

    Specifies the file to output the assembly to.  This option
    can be `-` to indicate that the assembly should be sent to
    standard output.

.. cmdoption:: input

    Specifies the input file to compile.  This option
    can be `-` to indicate that the input should be read from
    standard input.

.. _compiler-output-options:

Output options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -t type
            --type=type

    Specifies the output assembler to target.  See the section
    on :ref:`compiler-targets`.

.. cmdoption:: -e

    Specifies that the output object should be the entry-point
    of the application.  This is required for the time being
    while the C standard library is not yet written, but will
    in future be replaced when the standard library is available.
    See the section on :ref:`compiler-entry-point`.

.. _compiler-targets:

Assembler Targets
---------------------

Different assemblers support different directives and features.  The toolchain compiler allows you to target
a specific assembler thus preventing the compiler from outputting assembly code that will not compile
under the specified target assembler.

The following assembler targets are supported (as well as the known list of features they support):

+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+
| Type                  | .BOUNDARY | .DATA sections | .DAT instruction | Reordered addresses | Character literals | .IMPORT directive | .EXPORT directive |
+=======================+===========+================+==================+=====================+====================+===================+===================+
| dcpu16toolchain       | Yes       | `-`            | Yes              | Yes                 | Yes                | Yes               | Yes               |
+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+
| dcpu16studio          | `-`       | `-`            | Yes              | `-`                 | `-`                | `-`               | `-`               |
+-----------------------+-----------+----------------+------------------+---------------------+--------------------+-------------------+-------------------+

Each of the listed features is required to enable one-or-more components of language functionality.  Each feature
is described below:

+---------------------+--------------------------+---------------------------------------+
| Feature             | Purpose                  | Affected functionality                |
+=====================+==========================+=======================================+
| .BOUNDARY           | Provides a safety        | The compiler will fall back to using  |
|                     | boundary for debugging   | the .DAT directive to replicate this  |
|                     | purposes.                | functionality if it is not available. |
+---------------------+--------------------------+---------------------------------------+
| .DATA section       | Provides a contigious    | No current effect.                    |
|                     | block of data            |                                       |
|                     | declarations.            |                                       |
+---------------------+--------------------------+---------------------------------------+
| .DAT instruction    | Provides inline ability  | The compiler will refuse to compile   |
|                     | to embed raw data.       | without this functionality as it is   |
|                     |                          | impossible to embed string constants  |
|                     |                          | or any other non-numeric literal      |
|                     |                          | without it.                           |
+---------------------+--------------------------+---------------------------------------+
| Reordered addresses | Addresses can be used    | No current effect.  The compiler      |
|                     | in the form [REG+ADDR]   | always uses the standard format.      |
|                     | in addition to the       |                                       |
|                     | standard form            |                                       |
|                     | [ADDR+REG].              |                                       |
+---------------------+--------------------------+---------------------------------------+
| Character literals  | Embeds character         | The compiler will translate           |
|                     | literals such as 'a'     | characters to their ASCII values and  |
|                     | directly in the assembly | use those in the final resulting      |
|                     | to make the result more  | assembly instead.                     |
|                     | readable and             |                                       |
|                     | understandable.          |                                       |
+---------------------+--------------------------+---------------------------------------+
| .IMPORT directive   | Declares that a          | The compiler will disable the ability |
|                     | specified label will be  | to predeclare functions.              |
|                     | provided at link time.   |                                       |
+---------------------+--------------------------+---------------------------------------+
| .EXPORT directive   | Declares that a          | The compiler will not export          |
|                     | specified label should   | functions into the link table for     | 
|                     | be exported in the link  | use at link time.                     |
|                     | format and available     |                                       |
|                     | to other objects at link |                                       |
|                     | time.                    |                                       |
+---------------------+--------------------------+---------------------------------------+

.. _compiler-entry-point:

Entry Points
---------------------

Some languages require extra helping functionality when they are compiled, most commonly
advanced stack management.

Each language has it's own bootstrap assembly code, however, when compiling multiple source
files into their own objects, the bootstrap code should only be included once.

The `-e` option indicates that the bootstrap assembly should be attached to this
specific object that is produced.  At link time, this object must be specified as the first
object file for linker inputs.

See also :ref:`linker`.
