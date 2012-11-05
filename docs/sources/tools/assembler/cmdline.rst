.. highlightlang:: none

.. _assembler-command-line:

Command line
---------------

When invoking the assembler, you may specify any of these options

.. code-block:: bash

    dtasm [-rih] [-s symbols] -o output input

The most common use case is assembling intermediate code for later linking:

.. code-block:: bash

    dtasm -i -s test.dsym16 -o test.dobj16 test.dasm16

.. _assembler-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h

    Shows a list of all currently supported command-line options.

.. cmdoption:: -o output
            --output=output

    Specifies the file to output the bytecode to.  By convention,
    if you are using the intermediate code option `-i`, the extension
    of the resulting file should be `.dobj16` instead of `.dcpu16`
    (since the resulting file will not run directly on the DCPU).
    This option can be `-` to indicate that the result should be sent to
    standard output.

.. cmdoption:: input

    Specifies the input file to assemble.  This option
    can be `-` to indicate that the input should be read from
    standard input.  Specifying standard input can be used to chain
    assembler execution after the compiler.
    
.. cmdoption:: -v[vv]

    Increase verbosity.
    
.. cmdoption:: -q[qq]

    Decrease verbosity.

.. _assembler-output-options:

Output options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -r

    Specifies that a relocation table should be created as part of
    the resulting bytecode, according to the `relocation table draft <https://github.com/0x10cStandardsCommittee/0x10c-Standards/blob/master/ASM/Draft_Assembly_Relocation_Table.txt>`_
    submitted to the standards committee.
    
.. cmdoption:: -i

    Specifies that intermediate code should be generated instead of
    a single image.  When using this option, you are required to
    run the output through the :ref:`linker` in order to produce a
    valid DCPU image.  This option allows you to assemble multiple
    files individually and then combine them into a single image at
    link time.  Required for the .IMPORT and .EXPORT directives to work.

.. cmdoption:: -s symbols
                --debug-symbols=symbols

    Specifies the filename to write debugging symbols to.  This
    option does _not_ support `-` to write to standard output; the
    output will always go to a file.
