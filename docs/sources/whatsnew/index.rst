.. highlightlang:: none

.. _whatsnew:

What's new in DCPU-16 Toolchain?
=====================================

.. warning::

    This is written with Release Candidate 1 in mind.  It does not reflect the current state of the
    development tree.

Release Candidate 1 brings more functionality and more stability to the toolchain.  In particular, the notable
features of this release are:

Stability
-----------

This release has been heavily focused on bringing stability to the toolchain, with over 33 bug and feature reports
fixed affecting all components of the toolchain.  Here's a short list of some of the most serious issues fixed:

  * Linker will now compile multiple C files correctly (via the `-e` option).
  * Linker will now merge symbol information.
  * Font modification in the emulator now works correctly.
  * Preprocessor definition and expression system improved.

Quite a few edge-cases have also been resolved in this release.  Check the full list of `closed issues <https://github.com/DCPUTeam/DCPUToolchain/issues?milestone=2&page=1&state=closed>`_
for a more detailed list.

Documentation
----------------

Perhaps the most desired aspect to the toolchain was proper documentation.  You're reading it right now, so why
not have a look around?

Build Server
----------------

We now have a build server producing builds for each commit.  Check out the `build server <http://irc.lysdev.com:8010/>`_ if you're interested in living on the edge
or if you need a fix for an immediate problem.  Linux and Windows builds are provided by the build server, unfortunately Mac can not be cross-compiled for and thus
we are limited in the binary releases we can do for this platform.

Prefix / Postfix Support
---------------------------

The C compiler now supports prefix and postfix operations.

.. code-block:: c

    void main()
    {
        int i = 0;
        
        --i;
        ++i;
        i--;
        i++;
    }

Array Declaration Support
---------------------------

The C compiler now supports array declarations and array access operators.  See :ref:`compiler-lang-c-syntax` for
more information.

.. code-block:: c

    void main()
    {
        int myVar[3];
        int i = 0;
        
        myVar[0] = 5;
        myVar[1] = 10;
        myVar[2] = 15;

        for (i = 0; i < 3; i++)
            myVar[i] += 5;
    }

Signed Integer Support
---------------------------

The C compiler now supports signed integer math correctly.

.. code-block:: c

    void main()
    {
        int i = -200;
        
        i *= 2;
        
        // i is now -400
    }

Code Hotswapping
-------------------

The debugger is now capable of compiling and assembling code on-the-fly and hotswapping functions as needed.  This
allows you to modify and update functionality as you test it.
