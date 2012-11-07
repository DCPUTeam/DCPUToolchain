.. highlightlang:: none

.. _compiler-lang-c-syntax:

C Syntax
=============================================

Special Constructs
-------------------------

__asm block
~~~~~~~~~~~~~~~~~~~~~
The __asm block can be used to integrate DCPU assembly into the C code.

.. code-block:: c
    __asm {
        SET A, 0x1337
        SET B, 0xc0de
    }

Inside the `__asm` block, local C variables can be accessed:
.. code-block:: c
    void* screen = (void*) 0x8000;
    __asm {
        SET A, <screen>
        SET [A], 'X'
        BOR [A], 0xF000
    }

Formally the value of a variable `x` can be accessed with `<x>`.
You can also get the reference to a function with `<&someFunction>`.

    .. warning::
        Only local variables can be accessed. And only the value of the
        variable, not the reference can be used (i.e. no `<&x>`).
        
For using the value of global variables or the reference of local/global
variables, create a temporay copy in C code first:
.. code-block:: c
    int globalVar = 0xbeef;
    int main() {
        // set globalVar to 0xdead using __asm
        int* tmp = &globalVar;
        __asm {
            SET A, <tmp>
            SET [A], 0xdead
        }
    }




Unsupported Constructs
-------------------------
The C language compiler supports most C constructs.  Due to the rapid development nature of the toolchain, it's best to
determine support by simply trying something out and seeing whether or not it compiles successfully.

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
