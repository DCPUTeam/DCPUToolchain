How the DCPU-16 works
=================================

.. admonition:: Author Information
    :class: note
    
    This tutorial was originally written by SirCmpwn for 10c.co.

Now that you have your tools set up for use, now let's take a moment to talk about how the DCPU-16 actually works, and by extension, how the computer you're using now actually works!

.. note::

    This article is meant to introduce you to low-level concepts that apply to computing in general.  Refer to :ref:`tutorial-10cco-assembly` to skip it.

This article is awefully technical, but try to power through it - this is important stuff.

Required Reading
---------------------

I can't possibly teach you everything, so here's a list of useful things to read before we move on:

* `Binary <http://en.wikipedia.org/wiki/Binary_numeral_system>`_ - No need to read it all, but you should understand how it works.
* `Hexadecimal <http://en.wikipedia.org/wiki/Hexadecimal>`_ (aka "hex") - Ditto.
* `Signed Numbers <http://en.wikipedia.org/wiki/Signed_number_representations>`_ - How negative numbers work in computing.

.. note::

    I refer to "bits" a lot. The articles above don't explain it well - a bit is a binary digit. If I say a number is 4-bit, that means the maximum value is 1111 (in binary).  8-bit numbers can go up to 11111111, and so on.

What's in a DCPU
-------------------
Let's pretend for a moment that the DCPU-16 actually exists, and how it would be designed if it did. Your computer includes several things - the actual DCPU, 128 KiB of memory, and all the attached devices like screens and keyboards. Let's start with memory.

DCPU-16 Memory
-----------------

.. note::

    Sometimes, we use hexadecimal notation to refer to numbers (think back to that required reading above). These are notated with "0x" beforehand - 123 in decimal is written as 0x7B. We also tend to pad these until they're 4 digits long, like 0x007B, because the maximum number a DCPU can use with is 0xFFFF. We also refer to numbers in binary with "0b", such as 0b10101011.

So, your device contains 0x10000 (or 128 KiB) words of memory. This memory consists of a number of **words**. A word is a 16-bit unsigned number (between 0 and 65535, or 0x0000-0xFFFF). Each word is assigned an **address**, which you can use to refer to it. Typically, addresses are referred to in hexadecimal - some value between 0x0000 and 0xFFFF. For example, let's simplify it and assume that there are only 16 words of memory, and that they look like this (in hex):

.. code-block::

    0000 1234 0000 BEEF 0000 FACE 0000 4321 0000 0000 0000 0000 0000 0000 0000 0000

.. admonition:: Editor's Note
    :class: note

    The correct way to refer to the amount of memory the DCPU-16 has is "64kw" as-in, "64 kilowords".  It should not be referred to as 128kb of memory, as the DCPU-16 is not a byte-addressed CPU and thus the actual storage of 128kb of data is impractical.

We could think of the first value as being located at address ``0x0000``, and its value is ``0x0000``.  The value ``0x1234`` is located at address ``0x0001``, and the value ``0xBEEF`` is located at ``0x0003``, and so on.

In DCPU assembly, we usually refer to addresses by enclosing them in [brackets]. In the example memory, we could say that ``[0x0005]`` is equal to ``0xFACE``.

The CPU itself
----------------

The actual DCPU is responsible for manipulating this memory to execute programs and store data. Let's have a look at our example program from last time. Assemble it again, but this time, use this command: "organic.exe helloworld.dasm --listing helloworld.lst". Organic will generate a listing file that looks something like this (I've simplified this version a bit):

    [0x0000]               init:
    [0x0000] 1A00            HWN I
    [0x0001]               .loop:
    [0x0001] 88C3            SUB I, 1
    [0x0002] 84D2            IFE I, 0
    [0x0003] 8B83            SUB PC, 1
    [0x0004] 1A20            HWQ I
    [0x0005] 7C12 F615       IFE A, 0xf615
    [0x0007] 7C32 7349       IFE B, 0x7349
    [0x0009] B381            SET PC, .end
    [0x000A] 8B81            SET PC, .loop
    [0x000B]               .end:
    [0x000B] 8401            SET A, 0
    [0x000C] 7C21 8000       SET B, 0x8000
    [0x000E] 1A40            HWI I
    [0x000F]               start:
    [0x000F] 8401            SET A, 0
    [0x0010] 8421            SET B, 0
    [0x0011] 7C41 0024       SET C, message
    [0x0013] 7C20 0016       JSR print_string
    [0x0015] 8B83            SUB PC, 1
    [0x0016]               print_string:
    [0x0016] 7C24 0020       MUL B, 32
    [0x0018] 0402            ADD A, B
    [0x0019] 7C02 8000       ADD A, 0x8000
    [0x001B]               .loop:
    [0x001B] 8552            IFE [C], 0
    [0x001C] 6381            SET PC, POP
    [0x001D] 2821            SET B, [C]
    [0x001E] 7C2B F000       BOR B, 0xF000
    [0x0020] 0501            SET [A], B
    [0x0021] 8802            ADD A, 1
    [0x0022] 8842            ADD C, 1
    [0x0023] F381            SET PC, .loop
    [0x0024]               message:
    [0x0031]                 DAT "Hello, world!" 0
    [0x0031]                      0048 0065 006C 006C 006F 002C 0020 0077
    [0x0039]                      006F 0072 006C 0064 0021 0000

.. admonition:: Editor's Note
    :class: note

    The DCPU-16 toolchain uses symbol files to represent the information stored in an Organic listing file.  Symbol files have the benefit in that they are much easier for a computer to read and write, but they are not human-readable text files.  In order to view a symbol file, you must use the toolchain debugger's ``disasm`` command after loading a file like so: ``dtdb file.dcpu16 -s file.dsym16``.

This will be important later - we'll talk about it more in a moment. First, we need to talk about registers.

So, in addition to 0x10000 words of memory, we also have **registers**.  The DCPU-16 ships with 12 registers - 8 general purpose registers (referred to as A, B, C, X, Y, Z, I, and J), and 4 special purpose registers (referred to as PC, SP, EX, and IA). You can think of these as 16-bit variables that you can use in addition to memory. They can hold any value between 0x0000 and 0xFFFF, and you can use them to do various operations, like adding them together or storing them in memory. When the DCPU-16 starts up, all registers are set to zero.

Now that you know about those, let's look at that example file again. When it's loaded into the DCPU, the first bit of memory looks like this:

.. code-block::

    1A00 88C3 84D2 8B83 1A20 7C12 F615 7C32 7349 B381 8B81 8401 7C21 8000 1A40 8401
    8421 7C41 0024 7C20 0016 8B83 7C24 0020 0402 7C02 8000 8552 6381 2821 7C2B F000
    0501 8802 8842 F381 0048 0065 006C 006C 006F 002C 0020 0077 006F 0072 006C 0064
    0021 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000

Have a look at the listing file again - notice how the memory we have here corresponds to the values in each line of the listing. The assembler's job is to translate assembly code into these values, values that we then give to the DCPU to execute. Also pay special attention to things like "Hello world" - it's translated to the same stuff. And see those zeroes at the end? Keep in mind that this is just copied straight into memory, nothing special is done.

.. note::

    Quick tip: memory and programs are the same thing. You can set memory with the SET instruction, which will allow you to not only store data, but modify executable memory. This even allows for fancy tricks like **self-modifying code**, which lets you change your code while it's running!

This is where the CPU comes in. The CPU looks at the value at the address the PC register refers to (known as [PC]), and interprets it as an **instruction**, such as "HWN I". The CPU then **executes** that instruction, and moves on to the next one. Some instructions can do fancy stuff like moving PC somewhere else, or changing memory. So if PC is ``0x0000``, the CPU will execute ``[0x0000]``, which is ``0x1A00``.  ``0x1A00`` translates to ``HWI I``, so the CPU executes that instruction.

CPU Timing
--------------

Computers don't operate instantaneously. It takes time to execute each instruction. But how much? Well, the stock DCPU-16 is clocked to 100 kHz (kilohertz). That means that it executes a **cycle** 100,000 times per second. This is the basis for timing on DCPU-16. Each instruction takes a certain number of cycles to execute. For instance, take the SET instruction. SET is used for moving data, such as ``SET A, B`` to set the A register to the value of the B register (A = B).  ``SET A, B`` takes one cycle, or 10 microseconds. Each instruction has a base cycle count, and depending on how you use it, more cycles can be added. For example, when you load a constant number into a register, it can add another cycle, like ``SET A, 100``.  You can read the timings for all the instructions in the official `DCPU-16 specification <http://dcpu.com/dcpu-16/>`.

Let's see an example
-----------------------

Okay, so I've re-worked our example code to have a lot more detail. I explain what each thing does in the comments, which are ignored by the assembler. Comments are denoted by a semicolon (;), and continue to the end of the line. The code below shows these comments in gray - make sure you read them to get an idea of what's going on. Don't worry if you don't understand it all.

.. code-block:: nasm

    ; Hello World test program

    ; I removed a lot of the confusing code. We'll get to that a bit later.
    .INCLUDE <bootstrap.dasm> ; This just references the omitted code.

    start:
        ; This is where everything starts. This is the first thing that
        ; the CPU will execute. The assembler translates it to a binary
        ; file (just a fancy name for a file that's simple data), which
        ; is loaded into memory, and interpreted by the CPU. These first
        ; two instructions load zero into the A and B registers.
        SET A, 0
        SET B, 0
        ; This instruction loads the address of "message" into the C
        ; register. "message" is down at the bottom of this file.
        SET C, message
        ; This instruction runs a little routine that prints text to the
        ; screen. It prints it at the coordinates A, B (registers), and
        ; prints the text that the C register references (message).
        JSR print_string
        ; Finally, we subtract 1 from PC, which is a nifty way to loop
        ; forever and make the program hang.
        SUB PC, 1

    message:
        ; DAT is a special directive for the assembler that tells
        ; it to convert the data that follows and include it in the
        ; resulting binary file.
        DAT "Hello, world!", 0

And there you have it! In the next article, we'll help you get started writing your own DCPU-16 programs from scratch.
