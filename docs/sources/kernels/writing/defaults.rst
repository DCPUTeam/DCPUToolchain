.. _kernels-writing-defaults:

Defaults section
=============================

The default sections describes information about where to find the kernel and it's symbols
(for debugging).  It also describes how API calls should be made into the kernel.

.. code-block:: text
    
    defaults
    {
        kernel=@KERNEL_FOLDER@/stubsys.dkrn16
        symbols=@KERNEL_FOLDER@/stubsys.dsym16
        target=image
        direct-target=image-direct
        direct-default=false
    }

The general options in this section are:

.. py:attribute:: kernel
    :noindex:
    
    The path to the fully built kernel.  `@KERNEL_FOLDER@` will be replaced with the correct
    leading path when the policy is parsed.
    
.. py:attribute:: symbols
    :noindex:
    
    The path to kernel symbols.  These will be added to the resulting program's symbol
    database if the program is linked directly against the kernel.  Otherwise they are
    unused.  `@KERNEL_FOLDER@` will be replaced with the correct leading path when the
    policy is parsed.

.. py:attribute:: target
    :noindex:
    
    The default policy to use when linking against a kernel in a non-direct manner.
    See :ref:`kernels-writing-policies` for more information.
    
.. py:attribute:: direct-target
    :noindex:
    
    The default policy to use when linking against a kernel in a direct manner.
    See :ref:`kernels-writing-policies` for more information.
    
.. py:attribute:: direct-default
    :noindex:
    
    Whether or not the linker should directly link the program against the kernel.
    
    In a direct link, the kernel is added to the list of input files as the first file
    in the link.  The resulting binary is thus a complete copy of the kernel and the attached
    program.
    
    In a non-direct link (the default), the kernel is only referenced by the linker; it is
    not attached in the final resulting binary.
    
There are two types of API calls, via jumplists or via interrupts.

Jumplist-based kernels
----------------------------------

Jumplists are much more simple than using interrupts since they are in effect, just a fancy
JSR call.  When you write your kernel, you need to specify the position for the jumplist and
the labels that are exported in the jumplist.  As such, your `start.dasm16` file might look
something like this:

.. code-block:: nasm
    
    SET PC, _kern_start
    
    ; The jumplist will be placed here in the kernel.
    .JUMP
    ; The "my_exported_label" label will be made available via the jumplist.
    .JUMP my_exported_label
    ; The "another_exported_label" label will also be made available via the jumplist.
    .JUMP another_exported_label
    
    _kern_start:
        ; ... your code ...
        SET PC, _kern_halt
        
    _kern_halt:
        SET PC, _kern_halt
        
When you build this kernel with the toolchain, you will need to specify the output path for
a jumplist to be written to.  It will look something like this:

.. code-block:: bash
    
    dtld -l kernel --jumplist mykernel.djmp16 -o mykernel.dkrn16 -s mykernel.dsym16 ...
    
The associated linker policy for a jumplist-based kernel is the same as stubsys, and the defaults
section would look like:

.. code-block:: text
    
    defaults
    {
        kernel=@KERNEL_FOLDER@/mykernel.dkrn16
        jumplist=@KERNEL_FOLDER@/mykernel.djmp16
        symbols=@KERNEL_FOLDER@/mykernel.dsym16
    }
    
.. warning::
    
    The invocation of labels exported with a jumplist is always register-call (see the standards ABI).
    
The options available for jumplist-based kernels are:

.. py:attribute:: jumplist
    :noindex:
    
    The path to the kernel jumplist.  `@KERNEL_FOLDER@` will be replaced with the correct
    leading path when the policy is parsed.
    
Interrupt-based kernels
----------------------------------

Interrupts are more flexible than jumplists, in particular, they do not require the kernel to have
been built with the toolchain; they will work with any kernel at all.  As such, they require no
special directives to be added to the kernel source code.

However, the linker policy file for interrupt-based kernels are much more detailed as the linker must 
gather all required information from the "defaults" section of the linker policy.

A linker policy for an interrupt-based kernel may look like:

.. code-block:: text
    
    defaults
    {
        kernel=/home/james/0x10c-build/kernel/stubsys.dkrn16
        symbols=/home/james/0x10c-build/kernel/stubsys.dsym16
        direct=false
        
        # use stack call convention
        interrupt-call=stack-call
        
        # interrupt call mappings
        interrupt-call(_stubapi_malloc)=0x4001
        interrupt-call(_stubapi_free)=0x4002
        interrupt-call(_stubapi_errno)=0x4003
        interrupt-call(_stubapi_exit)=0x4004
        interrupt-size(_stubapi_malloc)=1
        interrupt-size(_stubapi_free)=1
        interrupt-size(_stubapi_errno)=0
        interrupt-size(_stubapi_exit)=1
    }
    
As you can see, the jumplist option is omitted and in it's place is a declaration of the type
of interrupt calls that are made (either "stack-call" or "register-call") and a list of mappings
from call names to their interrupt message IDs and the number of parameters they accept.

The options available for interrupt-based kernels are:

.. py:attribute:: interrupt-call
    :noindex:
    
    The type of interrupt calls to make.  Currently supported are "stack-call", "register-call"
    and "register-direct" each defining their own semantics about how parameters are passed to the kernel.
    
    For users of the .CALL directive this is irrelevant as the linker ensures that parameters are
    translated from an ABI register call, to a stack or register kernel call.
    
    .. note::
    
        The "stack-call" calling convention is one where all of the arguments to a
        kernel call are pushed onto the stack and the interrupt is triggered.  The
        result of the kernel call is in the position of the first argument when the
        interrupt returns and thus, a dummy value be pushed onto the stack if there
        are no arguments to the kernel call.
        
    .. note::
    
        The "register-call" calling convention is one where the first 3 arguments
        are put into the A, B and C registers respectively and then any additional
        arguments are pushed onto the stack.  The result of the kernel call is
        placed into the top of the stack and thus, if there are less than 4 arguments,
        a dummy value is pushed to hold the result.
        
    .. note::
        
        The "register-direct" calling convention is one where the first 3 arguments
        are put into the A, B and C registers respectively and then any additional
        arguments are pushed onto the stack.  The result of the kernel is placed
        into the position on the stack where RFI will restore from and thus upon
        returning from the interrupt, the DCPU-16 automatically sets the A register
        to the corect value.
        
    .. warning::
    
        A kernel "register-call" is *not* the same as an ABI register call since interrupts
        clobber the A register on the RFI instruction.
    
.. py:function:: interrupt-call(name)
    :noindex:
    
    Specifies the interrupt message ID that should be sent when calling the specified
    name via the .CALL directive.
    
    May be specified in either hexadecimal or decimal format.
    
.. py:function:: interrupt-size(name)
    :noindex:
    
    Specifies the number of parameters used when calling the specified interrupt message.

    May be specified in either hexadecimal or decimal format.    