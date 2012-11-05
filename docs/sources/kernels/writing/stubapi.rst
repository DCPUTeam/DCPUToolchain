.. _kernels-writing-stubapi:

Standard API (stubapi)
==================================

In order for a given peice of code to link against almost any kernel, there must be
a consistant API in place for performing operations with the kernel.  While kernel
developers may expose more than the minimum set of functionality and may expose
functionality beyond the scope of this API, there is a minimum set of functions that
must be provided for any kernel to be linked against.  These API functions are:

* malloc - Dynamically allocate memory
* free - Free previously allocated memory
* errno - The last reported error number
* exit - Terminate the process

.. warning::

    Since the API is quite early and likely to fluctuate as we pin down specifics, the current
    specification for what API calls are available can be found at https://github.com/DCPUTeam/DCPUToolchain/tree/master/kernel.
    
Whenever stubapi functions are referenced by labels or the linker, they have `_stubapi_`
prefixed to their names to ensure uniqueness.

Implementing a jump table handler
------------------------------------

Labels exported from the jump table require no manipulation for handling what-so-ever.
The label is called directly as it's name (e.g. `_stubapi_malloc`) and so no switching
based on IDs is required.  In addition, the context is already correct for ABI register
call, so you can immediately handle the request without manipulation of the DCPU-16 state.

Implementing an interrupt handler
-----------------------------------

When your kernel is interrupt based, you will have mappings from each of the API
functions to an interrupt message ID in your policy file.  For example, you might
designate `_stubapi_malloc` to be an interrupt message `0x3001`.

Depending on the calling convention, you will need to retrieve the parameters from
different points on the stack.  Since the `INT` instruction always moves A from the
register onto the stack, even if you are using the `register-call` or `register-direct`
calling conventions you will still need to retrieve this from the position on the stack.

Here is an example for handling the `register-call` convention:

.. code-block:: nasm
    
    ; At some point you set the handler..
    IAS register_call_handler
    
    ; Interrupt information storage.
    int_i_register:
        DAT 0x0000
    int_stack_pointer:
        DAT 0x0000
    int_return_address:
        DAT 0x0000
    
    ; Handle an interrupt.
    register_call_handler:
    
        ; Save all of our values.
        SET [int_i_register], I
        SET [int_stack_pointer], SP
        SET [int_return_address], [SP+1]
        
        ; Put the message ID into I.
        SET I, A
    
        ; Pull the value from the stack and put it
        ; back into the A register.
        SET A, PEEK
        
        ; We're going to temporarily move the stack
        ; pointer downwards so that when we perform
        ; these JSR calls, it's as if they were
        ; called in a normal register call environment.
        ADD SP, 2
        
        ; Now that we've set this up, we can simply
        ; chain jump to the correct handler for this
        ; interrupt.  Since we've put A back into it's
        IFE 0x3001, I
            JSR handle_stubapi_malloc
        IFE 0x3002, I
            JSR handle_stubapi_free
        ; .. etc ..
        
        ; Now that we've call the function, the result is
        ; in A.  First we restore our values though.
        SET SP, [int_stack_pointer]
        SET [SP], I ; Assuming those calls don't clobber I.
        SET [SP+1], [int_return_address]
        
        ; Now we need to determine the stack position to
        ; set A into based on how many parameters there were.
        ; We can safely use B now since we've made the call
        ; already.
        IFE 0x3001, I
            SET B, 1 ; _stubapi_malloc takes 1 parameter.
        IFE 0x3002, I
            SET B, 1 ; _stubapi_free takes 1 parameter.
        ; .. etc ..
        
        ; Set into the correct position.
        IFL B, 4
            SET PC, .small_set
        SET PC, .big_set
    
        ; The position is right above PC on the stack.
        .small_set:
            SET [SP+2], A
            SET PC, .finalize
            
        ; The position is somewhere above PC on the stack.
        .big_set:
            ; Safe to use C as well now.
            SET C, SP
            ADD C, 2
            ADD C, B
            SUB C, 3 ; This could be reduced, but is left as-is for clarity.
            
            SET [C], A
            SET PC, .finalize
            
        ; Finalize.
        .finalize:
            ; So now we've put A into the correct position on the stack, our
            ; stack pointer is back where it belongs.  We just need to restore
            ; I and then we can return!
            SET I, [int_i_register] 
        
            ; Always remember to call RFI.
            RFI
    