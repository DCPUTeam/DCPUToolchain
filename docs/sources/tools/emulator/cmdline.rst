.. highlightlang:: none

.. _emulator-command-line:

Command line
---------------

When invoking the emulator, you may specify any of these options

.. code-block:: bash

    dtemu [-dtlcvq] [-W policy]... [-e dump] [--little-endian] [-r n] program

The most common use case is executing a DCPU-16 program:

.. code-block:: bash

    dtemu program.dcpu16

.. _emulator-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h
            --help

    Shows a list of all currently supported command-line options.
    
.. cmdoption:: -W policy
    
    Modify the warning policies (not to be confused with linker policies).
    
.. cmdoption:: program

    Load in the program from the specified file.
    
.. cmdoption:: -e dump
            --execution-dump dump

    Create a full execution dump that records every state change that occurs in the DCPU-16
    to a file.
    
    .. warning::
    
        This file will get very large if you leave the emulation running for a long period
        of time.  It records all register states for every instruction the DCPU-16 processes.
        
.. cmdoption:: -d
            --debug
            
    Print out each instruction as it is executed.
        
.. cmdoption:: -t
            --show-on-terminate
            
    Show the DCPU-16 state when the emulation terminates due to an illegal operation.
    
.. cmdoption:: --little-endian
    
    For compatibility with older versions of the toolchain and old DCPU-16 programs.
    
.. cmdoption:: -v[vv]

    Increase verbosity.
    
.. cmdoption:: -q[qq]

    Decrease verbosity.
    
Emulation options
~~~~~~~~~~~~~~~~~~~~~~

.. cmdoption:: -r n

    Set the radiation factor to `n` (higher is less radiation).
    
.. cmdoption:: -c
    
    The DCPU-16 should catch fire when encountering an illegal operation
    rather than terminating execution.
