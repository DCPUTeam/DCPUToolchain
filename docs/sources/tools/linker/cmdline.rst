.. highlightlang:: none

.. _linker-command-line:

Command line
---------------

When invoking the linker, you may specify any of these options

.. code-block:: bash

    dtld [-hSvq] [-l target] [--keep-outputs] [--little-endian] [-O <level>] [--direct] [--no-direct] [--no-short-literals] [--symbol-extension=ext] [-s <file>] [-k <name>] [-p <file>] [--jumplist=<file>] [-W policy]... -o <file> <file> [<file>]...

The most common use case is linking multiple C objects into a single file:

.. code-block:: bash

    dtld -o test.dcpu16 fileA.dobj16 fileB.dobj16

.. _linker-generic-options:

Generic options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -h
            --help

    Shows a list of all currently supported command-line options.
    
.. cmdoption:: -W policy
    
    Modify the warning policies (not to be confused with linker policies).
    
.. cmdoption:: -o output
            --output=output
    
    Specifies the file to output the final DCPU image to.  Depending on the type of target that has
    been set, this image may be executable in the :ref:`emulator` and :ref:`debugger`, or it could
    be an image that has to be loaded by kernel.  Alternatively it could also be a static library
    which can be reused in a later link.
    
.. cmdoption:: input1 ... inputN
    
    Specifies the input files to link together.  The linker links these
    together in order and as such, the first file provided will be the
    entry point for the application.
    
    In C applications it is important that the first input file is `stdlib.dlib16`
    or the path to the C standard library.
    
    .. note::
        
        In the future, the compiler will automatically insert symbols to inform the linker of this requirement
        and thus linking against the standard library will become an automatic step.

Output options
~~~~~~~~~~~~~~~~~~

.. cmdoption:: -l target
            --link-as target
            
    Specifies the policy to use when building this image.  The available policies are based
    on the kernel that is being linked against (by default `stubsys`).
    
    Two options, `static` and `kernel` are treated specially.  `static` causes the linker
    to output a static library and `kernel` causes the linker to output a kernel image.
    
.. cmdoption:: --keep-outputs
    
    Causes the .OUTPUT directives to be maintained in the final resulting image.  This is
    used when generating standard libraries so that when code is linked against the static
    library, it can be moved into the correct position required for initialization and
    execution.
    
.. cmdoption:: --little-endian
    
    For compatibility with older versions and third-party emulators.
    
.. cmdoption:: --jumplist file

    The output path to export the jumplist to when linking as a kernel.  Only used if -l is set to
    `kernel`.
    
Kernel and policy options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
.. cmdoption:: -k name
            --kernel name
            
    Sets the name of the kernel to link against.  The kernel specified by this name will be searched
    for in the toolchain kernel path, which either is set-by-default in the case of deployed
    builds (see :ref:`toolchain-deployable`) or must be set with the :envvar:`TOOLCHAIN_KERNELS`
    environment variable.
    
.. cmdoption:: -p file
            --policy file
    
    The path to the policy file to use.  This can be used instead of -k for complete customization
    of the link process and is important when testing program linkage against in-development
    kernels.  See :ref:`kernels-writing`.

.. cmdoption:: --direct
    
    Perform a direct link against the kernel, overridding the kernel's default.  If the kernel
    does not support direct linking, this may result in an invalid program.
    
.. cmdoption:: --no-direct

    Do not perform a direct link against the kernel, overridding the kernel's default.  If the
    kernel does not support indirect linking, this may result in an invalid program.
    
Optimization options
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmdoption:: --no-short-literals

    Do not perform short literal optimization.  Disabling this will greatly speed up the linking
    process, but at the cost of a potentially much larger program.
    
.. cmdoption:: -O level
    
    Sets the optimization level which can be anywhere between 0 and 3.  0 indicates that
    no optimizations should be made.
    
.. cmdoption:: -S
    
    Optimize for speed over runtime size.  Useful for smaller programs where the overall
    program size has little impact.
    
    .. note::
        
        This option is allowed to make the resulting binary larger, as long as it uses
        less CPU cycles overall.

Debugging options
~~~~~~~~~~~~~~~~~~~~
    
.. cmdoption:: --symbol-extension ext
    
    The symbol extension that the input object files should be associated with.  This defaults
    to `.dsym16`.
    
.. cmdoption:: -s file
            --symbols file
    
    Produce a combined symbol file for the resulting binary.
