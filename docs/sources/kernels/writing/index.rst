.. _kernels-writing:

Writing kernels
=============================================

If you are currently developing a kernel for the DCPU-16, you can make it compatible with
the toolchain and eligible to be shipped with the toolchain by following this guide.

Linker policy files
---------------------

Linker policy files provide the linker with information about how to link against your kernel,
as well as instructions on how to create the final application binary.  You can specify any
kind of application format and you can also tell the linker to include adjustment symbols
so that applications can be relocated at runtime.

A linker policy file might look as simple as this like this:

.. code-block:: text
    
    defaults
    {
        kernel=@KERNEL_FOLDER@/stubsys.dkrn16
        jumplist=@KERNEL_FOLDER@/stubsys.djmp16
        symbols=@KERNEL_FOLDER@/stubsys.dsym16
    }

    format(image):
        offset 0x1000
        chain image-direct

    format(image-direct):
        write code
        
As you can see, this is the linker policy file for stubsys, the default kernel for linking code.  For
more information about the format of linker policy files, see these topics:

.. toctree::
   :maxdepth: 2
   
   defaults
   policies
   stubapi
   shared