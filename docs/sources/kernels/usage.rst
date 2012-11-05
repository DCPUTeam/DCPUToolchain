.. _kernels-usage:

Using kernels
=============================================

The toolchain contains support for automatically linking against any kernel independent
of what the kernel supports or what APIs the kernel exposes.

As a user writing code, it is important to understand how to call into the cross-kernel
APIs and what APIs are available.

.. warning::

    Since the API is quite early and likely to fluctuate as we pin down specifics, the current
    specification for what API calls are available can be found at https://github.com/DCPUTeam/DCPUToolchain/tree/master/kernel.

Calling API functions
--------------------------

To call a kernel function from assembly, use the .CALL directive, like so:

.. code-block:: nasm
    
    SET A, 0x20 ; allocate 20 words
    .CALL _stubapi_malloc
    ; pointer is now in A
    
That's all there is to it.  This code will work on any kernel that provides a linker policy.

Linking against kernels
--------------------------

If you are using the stable or master builds of the toolchain from the website, or have built
the toolchain in deployable mode (see :ref:`toolchain-deployable`), linking against a desired
kernel can be done so like:

.. code-block:: bash

    dtld -k mados -o program.dcpu16 input.dobj16 ...
    
If you are using a non-deployable version of the toolchain, you will either need to specify the
path to a policy file or set environment variables (see :ref:`toolchain-deployable`) in order to
reference the kernel correctly.  Thus your command line might look something like this:

.. code-block:: bash

    # Only do the export once..
    export TOOLCHAIN_KERNELS=/path/to/kernels
    dtld -k mados -o program.dcpu16 input.dobj16 ...

If you are developing your own kernel and want to test code against it, and you have a linker
policy defined, you can specify the policy file manually with:

.. code-block:: bash

    dtld -p /path/to/policy -o program.dcpu16 input.dobj16 ...

