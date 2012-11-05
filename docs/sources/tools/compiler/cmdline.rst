.. highlightlang:: none

.. _compiler-command-line:

Command line
---------------

When invoking the compiler, you may specify any of these options

.. code-block:: bash

    dtcc [-hvq] -o output [-t type] [-I directory] input

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
    
.. cmdoption:: -I dir

    Adds the directory `dir` to the directories to
    be searched for header files.
    
.. cmdoption:: -v[vv]

    Increase verbosity.
    
.. cmdoption:: -q[qq]

    Decrease verbosity.

.. _compiler-output-options:

Output options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -t type
            --type=type

    Specifies the output assembler to target.  See the section
    on :ref:`compiler-targets`.
