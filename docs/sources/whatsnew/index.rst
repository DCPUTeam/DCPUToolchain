.. highlightlang:: none

.. _whatsnew:

What's new in DCPU-16 Toolchain?
=====================================

.. note::

    Since the toolchain has adopted a contigious build system, this is an overview of some of the most recent,
    major features to appear in the toolchain.

The latest features to be implemented in the toolchain are:

Toolchain IDE
-------------------

Still in active development, there is now an open source IDE for the DCPU-16.  Currently featuring:

* Syntax highlighting for DCPU-16 assembly.
* Compilation of DCPU-16 assembly.
* Execution of the result binary with full debugging interface.

Linker Policies and Kernel Support
-------------------------------------

The toolchain now has support for linker policies which enable DCPU-16 assembly to be written
once and run under every toolchain-compatible kernel.  Linker policies allow for the advanced creation
of file formats, including :ref:`shared libraries <kernels-writing-shared>` and
:ref:`relocatable code <kernels-writing-policies>`.

New Preprocessor
--------------------

The toolchain now has a fully recursive preprocessor.  It includes:

* Different syntax sets for C and assembly.
* Full macro support in both C and assembly.
* Internally a better infrastructure for future developments.

C Compiler Improvements
--------------------------

The C compiler has seen major improvements.  It is now a 2-pass C compiler which can analyize errors before
attempting compilation.  This allows multiple errors to be reported with greater detail.

Variable argument lists are now supported.  It also has proper type support and checking, as well as the
inclusion of explicit type casting.  It also includes more standard library functions which integrate with
the new kernel systems.

Documentation
----------------

There is now documentation on the `internal toolchain APIs <http://dcputoolcha.in/docs/api>`_ which allows third-party developers
to build upon the toolchain libraries.

In addition, the user documentation has been brought up-to-date and is now considered to cover every aspect
of the toolchain.

Build Server Migration
-------------------------

The build server is now on a dedicated machine and resides at `http://bb.dcputoolcha.in:8080/ <http://bb.dcputoolcha.in:8080/>`_ .
