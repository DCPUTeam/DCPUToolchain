.. highlightlang:: none

.. _preprocessor-command-line:

Command line
---------------

The preprocessor is implicitly run for you when executing the assembler or compiler,
however you can use the stand-alone tool to see the final input evaluation that is being
sent to the compiler or assembler, which can help you to determine bugs arising in this
stage of compiling or assembling.

When invoking the preprocessor, you may specify any of these options

.. code-block:: bash

    dtpp [-h] input

The most common use case is evaluating a C file:

.. code-block:: bash

    dtpp test.c

The preprocessor always sends it's output to standard out.

.. _preprocessor-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h
    
    Shows a list of all currently supported command-line options.
    
.. cmdoption:: input
    
    Specifies the input file to preprocess.  This option
    can be `-` to indicate that the input should be read from
    standard input.
