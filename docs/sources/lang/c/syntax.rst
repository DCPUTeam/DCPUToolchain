.. highlightlang:: none

.. _compiler-c-syntax:

C Syntax
=============================================

The C language compiler supports most C constructs.  Due to the rapid development nature of the toolchain, it's best to
determine support by simply trying something out and seeing whether or not it compiles successfully.

Unsupported Constructs
-------------------------

The following constructs are currently unsupported in the C compiler:

Array Declarations
~~~~~~~~~~~~~~~~~~~~~

You can not currently declare arrays as part of the stack.  For example, the following will not compile
successfully:

.. code-block:: c

    void main()
    {
        int myVar[3];

        // code here
    }

Inline Variable Declarations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many C compilers have an extension to the standard that allows you to declare variables in the middle of
a function.  However, this is technically a feature of C++ and not a feature of C and hence the C compiler
does not (and will not) support this non-standard extension.  In the toolchain C compiler, you can not do:

.. code-block:: c

    void main()
    {
        int var;    /* Correct */

        if (true)
        {
            int b;  /* Incorrect */
        }
    }
    
Temporary Extensions
-------------------------

These extensions to C are temporary during the transitional period in which a full C standard library
is not yet available.  These extensions may *disappear at any time*, so you should not rely on them.

Debug Statement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is sometimes important to stop execution to inspect the given value of an expression at a particular
point in time.  The debug statement evaluates an expression, places the result in the A register and
halts program execution so that the state of the CPU can be inspected.

.. code-block:: c

    void main()
    {
        debug 4 + 5; /* A register will contain 9 */
    }
