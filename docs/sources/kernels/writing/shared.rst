.. _kernels-writing-shared:

How to: Shared libraries
==================================

You may want to include support for shared libraries in your kernel.  Shared libraries
allow applications to load and reference support functions without having to ship with
the functionality.  It also allows applications to automatically use newer versions of
functionality when it becomes available.

Shared libraries are a special format that will be defined in your kernel's policy file.
In order for the kernel to be able to determine what label is desired by a program, you
will need to ensure the shared library contains at least:

* adjustment entries - So the shared library can be loaded into memory at any address.
* provided entries - So the kernel knows where labels are inside the shared library.

.. graphviz::
    
    digraph LibraryModel {
        Application -> "(1) .CALL _stubapi_dynload";
        "(1) .CALL _stubapi_dynload" -> Kernel;
        Kernel -> "(2) Reads library header";
        "(2) Reads library header" -> "Shared Library";
        Kernel -> "(3) Locates label address";
        "(3) Locates label address" -> "Shared Library";
        Kernel -> "(4) Returns the address";
        "(4) Returns the address" -> Application;
        
        "(1) .CALL _stubapi_dynload" [shape=plaintext];
        "(2) Reads library header" [shape=plaintext];
        "(3) Locates label address" [shape=plaintext];
        "(4) Returns the address" [shape=plaintext];
    }
    
Application code
------------------------

The application code is very simple.  Knowing ahead of time the parameters and return
information of the function you want to call, you could write the following code:

.. code-block:: nasm
    
    SET PC, start
    
    ; Define the library and function names.
    libname:
        DAT "mymath.so" 0
    funcname:
        DAT "add_numbers" 0
    
    ; Define somewhere to store the function pointer
    ; after we have it, since we don't want to perform
    ; the lookup for every call.
    funcptr:
        DAT 0x0000
    
    ; The start of our code.
    start:
    
        ; ------ Load function in shared library ------
    
        ; First we load the parameters to call the kernel with.
        SET A, libname
        SET B, funcname
        
        ; Call the _stubapi_dynload API function in the kernel.
        .CALL _stubapi_dynload
        
        ; Save the pointer to the function.  As an idea, the kernel
        ; could return 0x0000 if the function wasn't found and then
        ; we could check / handle that.
        SET [funcptr], A
        
        ; ------ Call the function in the shared library ------
        
        ; Load the parameters for the function.
        SET A, 25
        SET B, 125
        
        ; Call the function.
        JSR [funcptr]
        
        ; Assuming the function does what it says..
        ; A will now be 125.  Store the result in I.
        SET I, A
        
        ; We could call the function again...
        SET A, 30
        SET B, 45
        JSR [funcptr]
        ADD I, A
        
        ; A is now 200 (30 + 45 + 125).
        SET PC, halt
        
    ; The standard halting loop.
    halt:
        SET PC, halt
    
Kernel code
------------------------

The work that the kernel has to do in order to read a shared library format depends heavily on
your exact implementation and thus we don't provide a code example here.  Things that need to
be considered are:

* Your shared library format specified in the linker policy file.
* How you read files from disk.
* How you load shared libraries into memory.

The only mechanism that is consistant is that when you find a matching entry, you get the address
stored in that entry, adjust it for where the shared library is actually loaded into memory
(since they'll be relative to 0x0) and then return it as part of the kernel call handler to
the application.

Shared library
----------------------

The shared library doesn't actually have to execute any code until such point as the application
calls the function pointer.

At no point during this process does the shared library execute code, unless you've defined a specific
label in your format that your kernel recognises as an initialization function to call.  This is
similar to how other operating systems permit libraries to run initialization and finalization code
when they are loaded / unloaded.
