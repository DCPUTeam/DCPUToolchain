.. highlightlang:: none

.. _compiler-command-line:

Command line
---------------

When invoking the compiler, you may specify any of these options

.. code-block:: bash

    dtcc [-eh] -o output [-t type] input

The most common use case is compiling a C file into assembler:

.. code-block:: bash

    dtcc -o test.dasm16 test.c

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