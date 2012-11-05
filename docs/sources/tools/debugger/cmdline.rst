.. highlightlang:: none

.. _debugger-command-line:

Command line
---------------

When invoking the debugger, you may specify any of these options

.. code-block:: bash

    dtdb [-hnvq] [-c command] [-s symbols] [program] [--little-endian]

The most common use case is loading a DCPU-16 program with symbols:

.. code-block:: bash

    dtdb -s program.dsym16 program.dcpu16

.. _debugger-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h
            --help

    Shows a list of all currently supported command-line options.
    
.. cmdoption:: -n
            --no-attachment

    Do not attach default devices when launched.
    
.. cmdoption:: -c command

    Run a single command and then exit.
    
.. cmdoption:: -s file
            --symbols file

    Load in symbols from the specified file.
    
.. cmdoption:: program

    Load in the program from the specified file.
    
.. cmdoption:: --little-endian
    
    For compatibility with older versions of the toolchain and old DCPU-16 programs.
    
.. cmdoption:: -v[vv]

    Increase verbosity.
    
.. cmdoption:: -q[qq]

    Decrease verbosity.
    