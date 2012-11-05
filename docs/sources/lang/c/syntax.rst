.. highlightlang:: none

.. _compiler-lang-c-syntax:

C Syntax
=============================================

The C language compiler supports most C constructs.  Due to the rapid development nature of the toolchain, it's best to
determine support by simply trying something out and seeing whether or not it compiles successfully.

Unsupported Constructs
-------------------------

The following constructs are currently unsupported in the C compiler:

Type Definitions
~~~~~~~~~~~~~~~~~~~~~

You can not currently create type definitions:

.. code-block:: c

    typedef blah_t int;

    void main()
    {
        blah_t myVar;

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
