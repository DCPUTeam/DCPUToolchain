Getting a feel for Assembly
=================================

.. admonition:: Author Information
    :class: note
    
    This tutorial was originally written by SirCmpwn for 10c.co.

You've got the tools, you understand more about how the DCPU works, and now it's time to have a little fun. I'll walk you through the steps in creating your very own DCPU-16 programs.

10c.co Starter Kit
-----------------------

Assembly is a little daunting at first, and DCPU-16 doesn't make it much easier - namely with regard to hardware initialization and basic routines. To make this easier on you, I've prepared a little assembly kit for you to use while you're learning so you can save the complicated stuff for a few articles down the line.  You can download it from `this link <../../../_static/starter-kit.zip>`_.

How does it work, you ask? Well, first unzip it and we'll take a look around.  In ``extras/``, we have some helper code to do some heavy lifting for you.  In the root is ``template.dasm``, which is a template to make it easier for you to write code for, and ``hello-world.dasm``, which is an example program using the starter kit.

.. admonition:: Editor's Note
    :class: warning

    The mirrored starter kit omits the tools and scripts to ensure that you always get the latest version of a toolchain.  You will need to select a toolchain to use from the :ref:`tutorial-getting-tools` tutorial.

Exploring the Starter Kit
----------------------------

We'll be starting out by recreating that hello world example from scratch, using the starter kit.  Copy ``template.dasm`` to ``lesson1.dasm``, and open it up in a text editor. It should look something like this:

.. code-block:: nasm

    ; Template for 10c.co DCPU-16 tutorials

    ; Include some boilerplate
    .INCLUDE <boilerplate.dasm>

        ; Add your code here
        
    end:
        SET PC, end ; Loop forever

Let's have a look at this. Ignoring comments, the first line is this:

.. code-block:: nasm

    .INCLUDE <boilerplate.dasm>

To the assembler, ``.INCLUDE`` means "insert the contents of a file into this one." In this case, it's using ``boilerplate.dasm``.  Note the ``<>`` around it - that means to load it from the include path, which the starter kit has set to ``extras/``.

.. admonition:: Editor's Note
    :class: warning

    Since the starter kit no longer provides scripts, you will need to pass ``-I`` or the appropriate option to the assembler to specify the path to the ``extras/`` folder.

Next, we have the self-evident ``; Add your code here``.

Finally, the simple template ends with this:

.. code-block:: nasm

    SET PC, end

This code loops around. When you see something like ``end:`` in code, it's a **label**.  Basically, the assembler remembers the address of that label and allows you to use it again later in assembly, like we do on the next line. As you might guess ``SET PC, end`` loops forever, and that's how we're going to end our program. Let's go ahead and add in some of our own code.

Saying Hello
------------------

It's time to greet the user. We'll need to learn about two instructions to do this:

* ``SET register, value``: Sets the value of one ``register`` to a constant ``value``.
* ``JSR address``: Runs the ``address`` function. We'll describe this more later.

Included in the starter kit is a function called ``print_string``, which will output text to the screen.  It outputs the text at [A], at X,Y on the screen, where A, X, and Y are registers.

But before we can use that, we need to add a string to be displayed. We'll use the assembler ``DAT`` directive, which translates text into data that print_string can use in the final output. Add this to the end of the file:

.. code-block:: nasm

    message: DAT "Hello, world!" 0

.. note::

    The 0 suffix is used as the terminator in C-style strings.  Without it, the print_string function doesn't know when to stop reading data.

.. warning::
    
    Make sure you add this *after* the ``end:`` loop.  Code is just a special kind of data - you could accidentally run "Hello, world!" like code!

Now that the string is included, we can display it with print_string. After ``; Add your code here``, add the following code:
    
.. code-block:: nasm

    SET X, 0
    SET Y, 0
    SET A, message
    JSR print_string

Can you tell what this does?  Rememeber that print_string wants the X and Y positions in their respective registers, and the text in A. So, we set each register to those values, and then we can draw the string with JSR print_string.

Wrapping Up
------------------

Now that you've got your first program written, play around with it! Change the text, and move it around the screen. Maybe you could write several lines of text? Try to get a feel for playing around with the assembly. You should also try out some other routines from the starter kit, like print_hex, which is the same as print_string, except that it outputs the value of A in hexadecimal. There's print_dec (decimal), too! Maybe you could make a simple loop that counts up from 0 forever?

I suggest that you have a look at the `official specification <http://dcpu.com/dcpu-16/>`_ and play around a bit with the instructions listed there.

Here's the full code of the hello world demo:

.. code-block:: nasm
    
    ; "Hello, world!" for 10c.co DCPU-16 tutorials

    ; Include some boilerplate
    .INCLUDE <boilerplate.dasm>

        ; Add your code here
        SET X, 0
        SET Y, 0
        SET A, message
        JSR print_string
        
    end:
        SET PC, end ; Loop forever
        
    message:
        DAT "Hello, world!", 0

