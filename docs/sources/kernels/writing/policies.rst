.. _kernels-writing-policies:

Writing individual policies
==================================

Policies (not to be confused with policy files) define how the
linker should put together application files with building against
a specific kernel.

They allow kernels to define file format for storing code, including
the ability to define executable and shared library formats.

The best demonstration of the functionality of linker policies is
straight from the provided example:

.. code-block:: bash

    # The defaults section would go here in a full policy file.

    # This is an example of a policy for a kernel that always loads
    # code into the same location.  The code can be directly copied
    # into that location and executed without relocation.
    format(fixed):
        # Offset all references to be relative to 0x1000.
        offset 0x1000
        # Write out the code to the file.
        write code
    
    # Policies also support "chaining", where by one policy can invoke
    # another (just make sure you don't define them recursively).
    
    # This is an internal policy which is used in the relocatable and
    # shared policies below.  It can't be used directly as a parameter
    # to -l on the linker command line.
    internal(write-adjustment):
        # Write out the total number of entries in the adjustment table.
        write total(adjustment)
        # Write out the address field for each entry.
        for $i from 0 to total(adjustment)
            write field(adjustment, $i, address)
        endfor

    # This is another internal policy used for writing out entries of
    # provided labels and their addresses (for use in shared or
    # dynamically loaded libraries).
    internal(write-provided):
        # Write out the total number of entries in the provided table.
        write total(provided)
        # Write out each entry in the provided table.
        for $i from 0 to total(provided)
            write field(provided, $i, label_size)
            write field(provided, $i, label_text) # Important! This is an unpacked string.
            write field(provided, $i, address)
        endfor
        
    # Here we are defining a shared library format that could be used
    # to write out shared libraries.
    format(shared):
        # Write out a magic header so the kernel can verify that what
        # it's about to load is actually a shared library.
        write words(0x8A5F, 0x68FF)
        # Use our internal policies above to write out the required
        # information.
        chain write-adjustment
        chain write-provided
        # And then write out the code.
        write code

    # Here we are defining a relocatable application.  By providing the
    # kernel with a list of adjustments, it knows immediately what words
    # to adjust before executing the code.  Since the code is by default
    # at an origin of 0x0, the kernel can do a simple add operation on each
    # word to relocate the program.
    format(relocatable):
        # Write out the magic header.  Obviously it should be different
        # to the shared library!
        write words(0x1234, 0x5678)
        # We only need to write out the adjustment table this time.
        chain write-adjustment
        # And then write out the code.
        write code
        
Syntax structure
-------------------------

Policy types
~~~~~~~~~~~~~~~~~~

There are two policy types: "format" and "internal".  Format may be used by the
`-l` option on the linker command line, where as attempting to link with an
internal policy will fail.

Both policy types accept a series of instructions.

Policy instructions
~~~~~~~~~~~~~~~~~~~~~~~

There are four types of policy instructions.

.. py:attribute:: write
    :noindex:
    
    This instruction accepts a list of words and writes them to the output file.  Any
    function that returns a list of words can be used as the parameter.
    
.. py:attribute:: offset
    :noindex:
    
    This instruction accepts a numeric value and causes all of the code in the resulting
    file to offset to this address, as if a global `.ORIGIN` directive was issued.  Subsequent
    calls to `offset` will negative the effects of the first.
    
.. py:attribute:: chain
    :noindex:
    
    This instruction accepts the name of another policy (internal or otherwise) and
    executes it.  Once the chained policy has finished executing, control returns to the
    current policy.
    
.. py:attribute:: <for loop>
    :noindex:
    
    This instruction runs a for loop over the section of instructions.  The syntax of the
    for loop is shown in the example above; the first parameter must be a variable name (prefixed
    with a $ sign), the second and third parameters can be a numeric value or any function that
    returns a numeric value.
    
    The designated variable exists only for the scope of that for loop and the variable must not
    already exist when the for loop is encountered.  The variable is incremented by 1 each iteration.
    
Policy functions
~~~~~~~~~~~~~~~~~~~~~
    
.. py:function:: code
    :noindex:
    
    Although there are no brackets, this is a function that accepts no arguments.  It returns a list
    of words that are the words to be written to file.
    
.. py:function:: total(table)
    
    Returns the total number of entries in the specified table.
    
.. py:function:: field(table, index, field)
    
    Returns a list of words associated with the data in the specified field, in the specified row (by
    index) in the specified table.
    
.. py:function:: words(...)
    
    Accepts a list of numeric arguments or function calls that return numeric values and strings them
    together in a list of words suitable for passing to the write instruction.

Policy tables and fields
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The policy tables and fields align with the tables and fields available in link objects (since this is what
policies operate on).

The table names are "adjustment", "provided", "required", "section", "output", "jump", "optional" and "call".
The field names are "label_size", "label_text" and "address".

.. note::
    
    The label field in an "adjustment" table entry is always blank.
    
