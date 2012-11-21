Getting the tools for DCPU-16 Programming
===============================================
   
.. admonition:: Author Information
    :class: note
    
    This tutorial was originally written by SirCmpwn for 10c.co.

If you're ready to learn how to work with the DCPU-16, you'll need to get the tools.  First let's describe a few things:

* **Code** - You'll be writing code in assembly that's stored as plaintext files locally.
* **Assembler** - Assemblers are tools that turn your assembly code into an executable binary.
* **Emulator** - An emulator simulates a DCPU-16 in order to execute binary files from the assembler.

You'll need to get a handle on each of these things.

.. note::

    You need a text editor, too.  For Windows, I suggest `Notepad++ <http://notepad-plus-plus.org/>`_.  For Mac, I suggest
    `Sublime Text <http://www.sublimetext.com/>`_.  If you're on Linux, you probably already have a favourite text editor.

Choosing an Assembler
-------------------------

There are a huge number of assemblers out there to choose from, but most of them are pet projects that aren't practical for normal use. Your real choice these days boils down to two options:

* `Organic <https://github.com/0x10c-crap/Organic>`_ - Written by myself, this assembler has the most extensibility and flexibility of any assembler out there.
* `DCPU-16 Toolchain <http://dcputoolcha.in/>`_ - A strong collaborative effort, the toolchain offers a large number of tools written in C.

.. admonition:: Editor's Note
    :class: warning

    As of re-mirroring, Organic is no longer maintained (as with 10c.co).  Users are advised that due to this, it may not be up-to-date with the latest DCPU-16 specifications.

Choosing an Emulator
-------------------------

Again, there are a huge number of emulators out there, but most of them aren't too great. It again boils down to basically two choices:

* `Lettuce <https://github.com/0x10c-crap/Tomato>`_ - A powerful graphical debugger that can integrate deeply with Organic to provide a GUI debugging experience.
* `DCPU-16 Toolchain <http://dcputoolcha.in/>`_ - The toolchain debugger is a powerful command-line based tool with a good Lua scripting interface.

Organic and Lettuce combined are part of the BLT stack. You can also mix and match tools from the toolchain and the BLT stack, or most other assemblers and emulators.

Testing out the Tools
-------------------------

Once you've downloaded your tools, create a directory to work in and place them there. Let's test them out by creating a file called "helloworld.dasm" and putting the following code within. Don't worry if you can't understand it, we're just trying to make sure everything is set up properly.

.. code-block:: nasm

    ; Hello World test program

    init:
        ; Set up screen
        ; Find the number of connected devices
        HWN I
    ; You should use a local label whenever you have something generic like "loop"
    .loop:
        ; Loop through all connected devices in search of
        ; a LEM-1802 screen.
        SUB I, 1
        IFE I, 0
            SUB PC, 1 ; Loop forever if there are no screens
        HWQ I ; Query the device
        IFE A, 0xf615
            IFE B, 0x7349
                SET PC, .end ; We got a screen to use
        SET PC, .loop
        
    .end:
        ; I is the index of a LEM-1802
        SET A, 0
        SET B, 0x8000
        HWI I ; Map the screen to 0x8000
        
    start:
        SET A, 0
        SET B, 0
        SET C, message
        JSR print_string ; Call the print_string routine to write to the screen
        
    .exit:
        SET PC, .exit ; Loop forever
        
    ; Print the string in [C] to A,B
    print_string:
        MUL B, 32
        ADD A, B
        ADD A, 0x8000
    .loop:
        IFE [C], 0
            SET PC, POP
        SET B, [C]
        BOR B, 0xF000
        SET [A], B
        ADD A, 1
        ADD C, 1
        SET PC, .loop

    message:
        DAT "Hello, world!" 0

Save this file and drag the file on to ``Organic.exe``.  You'll see ``helloworld.bin`` appear, and you can drag this on to ``Lettuce.exe``.  Click accept on the window that appears, then press F5, and you should see the screen say "Hello, world!".

If you're on Linux or Mac, you probably can't just drag and drop these. Try running this from a terminal: ``mono Organic.exe helloworld.dasm`` and ``mono Lettuce.exe helloworld.bin``.
