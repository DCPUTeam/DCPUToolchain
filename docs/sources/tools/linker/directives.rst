.. highlightlang:: none

.. _linker-directives:

Directives
======================

Some assembler directives cause different behaviour at the linking stage.  You must
pass `-i` to the assembler to enable intermediate code generation mode in order to
use these directives.

.. _linker-import-export:

Cross-file references
------------------------

You can import and export labels between files by using the `.IMPORT` and `.EXPORT`
directives.

.. asmdirective:: .IMPORT [OPTIONAL] label

    Imports the specified label from an external object at link
    time.  The label will only be correctly imported if it was
    exported from another object file using the .EXPORT directive.
    
    `OPTIONAL` can also be added between the `.IMPORT` and the label,
    indicating that if this label is not available at link time, rather
    than exiting with an error, any references to the label should
    be replaced with 0x0000.

.. asmdirective:: .EXPORT label

    Exports the specified label from the the current file, making
    it available to other objects at link time.

This allows the following code to work:

.. code-block:: nasm

    .IMPORT label
    JSR label

.. code-block:: nasm

    .EXPORT label
    :label
        SET A, 5

.. _linker-sections:

Sections
--------------

When understanding linker sections it is first important to understand how the linker
performs it's job.

The linker initially reads in object code from an object file, and
this object file contains at the very start a linker table which tells the linker labels
that are required for this object to link, labels that are provided by this object,
addresses that should be adjusted and importantly sections and output areas that are
defined.

To handle this complex process, the linker places code into "bins".  These bins store
object code relative to the start of the bin (this is one reason that internal label
references in assembly can't refer to labels in other sections).

But what use do sections have?  There are many uses for sections but the most critical
is from within the C compiler, where global C variables must be initialized before
`main()` is called.  But there's one problem!  Global C variables and their initializers
can be declared in any C file and the `.IMPORT` / `.EXPORT` mechanism only allows the
C file to expose the existance of the global variable, it doesn't provide any mechanism
to call the initializer.  There's no way for the C file that contains `main()` to ensure
that all the global initializers are executed before `main()`.  Enter sections.

Sections allow you to define areas within assembly that should be named, like follows:

**a.dasm16:**

.. code-block:: nasm

    SET A, 0x5
    .SECTION INIT
    SET A, 0x10
    .SECTION CODE
    SET A, 0x15

**b.dasm16:**

.. code-block:: nasm

    SET A, 0x20
    .SECTION INIT
    SET A, 0x25
    .SECTION CODE
    SET A, 0x30

**c.dasm16:**

.. code-block:: nasm

    SET A, 0x35
    .OUTPUT INIT
    SET A, 0x40
    .OUTPUT CODE
    SET A, 0x45

In A and B we have defined INIT and CODE sections in both of these files.  In C we tell
the linker that these concatenated sections should be outputted at the respective locations.

Thus the result of running `dtld -o o.dcpu16 c.dobj16 a.dobj16 b.dobj16` (the order is important)
will be:

.. code-block:: nasm
    
    SET A, 0x35 ; start of C
    SET A, 0x10 ; init of A
    SET A, 0x25 ; init of B
    SET A, 0x40 ; middle of C
    SET A, 0x15 ; code of A
    SET A, 0x30 ; code of B
    SET A, 0x45 ; end of C
    SET A, 0x5  ; non-section of A
    SET A, 0x20 ; non-section of B

The code located in a section is appropriately placed in the correct output location by the
linker.  Any additional code that is not defined in a section is outputted in order, thus the
reason that order of linked files is important.  If we were to specify A and B before C, then the
`non-sections of A and B` would appear before `start of C` since they are handled by the linker
first.

The directives relevant to section usage are as follows:

.. asmdirective:: .SECTION name

    Defines that from this point onwards, code is owned by the section
    specified by `name`.

.. asmdirective:: .OUTPUT name

    At link time, the accumulative code that has been placed in the
    `name` section should be outputted at this point.
    
    To prevent circular dependencies, .OUTPUT may not be placed in a section
    and thus, no .SECTION directives are permitted before .OUTPUT.

Jumplists
---------------

You can export a jumplist when producing a kernel and you can import a jumplist entry
when linking in non-kernel mode.  For exporting a jumplist in kernel mode, see
:ref:`Jumplist-based kernels <kernels-writing-defaults-jumplist>`.

When linking in non-kernel mode, you can import a jump entry like so:

.. code-block:: nasm
    
    .JUMP something
    
    ; Call something.
    JSR [something]
    
.. warning::
    
    Application code should not directly reference jumplists in all but exceptional circumstances.
    To call into kernel functions, applications should use the `.CALL` directive as outlined below.
    
Kernel calls
--------------

The `.CALL` directive calls an API function exposed by the kernel.  How the call is performed is
dependent on the current kernel and it's linker policy.

When using the `.CALL` directive, users must use the register-call calling convention as outlined
in the `ABI standard <https://github.com/0x10cStandardsCommittee/0x10c-Standards/blob/master/ABI/ABI%20draft%202.txt>`_.

For more information on usage, see :ref:`kernels-usage`.

