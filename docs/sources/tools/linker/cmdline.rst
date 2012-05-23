.. highlightlang:: none

.. _linker-command-line:

Command line
---------------

When invoking the linker, you may specify any of these options

.. code-block:: bash

    dtld [-h] -o output input1 ... inputN

The most common use case is linking multiple C objects into a single file:

.. code-block:: bash

    dtld -o test.dcpu16 fileA.dobj16 fileB.dobj16

.. _linker-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h

    Shows a list of all currently supported command-line options.

.. cmdoption:: -o output
            --output=output
    
    Specifies the file to output the final DCPU image to.  This image
    will be executable in both the :ref:`emulator` and :ref:`debugger`.
    
.. cmdoption:: input1 ... inputN
    
    Specifies the input files to link together.  The linker links these
    together in order and as such, the first file provided will be the
    entry point for the application.
    
    In C applications it is important that the first input file was
    created using the `-e` option on the compiler.  See :ref:`compiler-entry-point`
    for more information.

Output options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -l target
               --link-as target

    Specifies the type of target output to produce.  Can be either "image"
    or "static".

    "image" indicates that the linker should produce a final DCPU-16 image for
    execution in either the emulator or debugger.  This is the functionality that
    the linker provides by default.

    "static" indicates that the linker should produce a static library that
    can be used to relink later on.  Essentially this option combines multiple
    object files into a single object file, although the naming convention for
    the result of this linking process is to end the file with the ".dlib16"
    extension instead of ".dobj16" (to signify intended usage).
