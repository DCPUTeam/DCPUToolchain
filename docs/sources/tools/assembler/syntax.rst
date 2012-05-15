.. highlightlang:: none
   
.. _assembler-syntax:

Syntax
---------------

The DCPU-16 Toolchain provides additional syntax constructs that may not be found
elsewhere in other assemblers.  Below is an outline of all of the supported syntax.

.. _assembler-directives:

Directives
~~~~~~~~~~~~~~~~~~

These directives are supported in addition to the directives provided
by the :ref:`preprocessor`.

.. asmdirective:: .BOUNDARY

    Creates a debugging boundary that can be used to visually seperate
    produced code when inspecting the resulting bytecode.  This directive
    inserts 16 NULL bytes into the resulting binary which not only
    allows the seperation of code to be easily seen, but is also useful
    to ensure that code execution does not leak out beyond the expected
    bounds.

.. asmdirective:: .EXTENSION extension

    Defines that the specified extension should be enabled when the
    resulting code is run in an emulator or debugger.  Has no effect
    when running inside 0x10c itself.  See the `extension table draft <https://github.com/0x10cStandardsCommittee/0x10c-Standards/blob/master/ASM/Draft_Extension_Declaration_Table.txt>`_
    for more information.

.. asmdirective:: .ORIGIN address
            .ORG address
    
    Defines that the assembler should seek to the specified address
    in the output file without regard to whether the address has
    previously been hit or whether the address is beyond the end of
    the current output.  If the address is beyond the current output,
    NULL bytes are written until the address is reached.

.. asmdirective:: .INCBIN "path"

    Includes the specified path literally, without modification, as
    a binary include.

.. asmdirective:: .FILL total character

    Outputs `character`, `total` times into the result.  Acts as a
    repeated :asminst:`DAT` directive.

.. asmdirective:: .IMPORT label

    Imports the specified label from an external object at link
    time.  The label will only be correctly imported if it was
    exported from another object file using the .EXPORT directive.

.. asmdirective:: .EXPORT label

    Exports the specified label from the the current file, making
    it available to other objects at link time.

.. _assembler-instructions:

Instructions
~~~~~~~~~~~~~~~~~~

All instructions in DCPU assembly take either 1 or 2 parameters, with
the exception of :asminst:`DAT`, which is a meta-instruction that
accepts literals.

All instructions specified in `revision 1.7 of the DCPU-16 specification <http://pastebin.com/raw.php?i=Q4JvQvnM>`_
are supported directly in the assembler.

The parameters for instructions are seperated with commas.
    
.. _assembler-literals:

Literals
~~~~~~~~~~~~~~~~~~

The assembler supports the following types of literals:

  * **Numbers:** 45
  * **Characters:** 'a'
  * **Strings:** "this is my text"
  * **Labels:** myLabel
  * **Opcodes:** {ADD} {SUB} ... etc ...
  * **Registers:** {REG:A} {VAL:A} {NXT:A} {REG:B} {VAL:B} ... etc ...
  * **Specials:** {POP} {PUSH} {PEEK} {PICK} {SP} {PC} {EX} {IA} {NXT}
  * **Expressions:** 4 + 5 (see also :ref:`preprocessor-expressions`)
