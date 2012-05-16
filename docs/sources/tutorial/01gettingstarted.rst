Getting Started
=====================

Download the latest stable version from the DCPU-16 Toolchain website:

    http://dcputoolcha.in/

Extract the contents of the archive to any folder you like, although a folder without
spaces in the path will most likely make it easier to work with.

You now need to set up your IDE or text editor to use the tools.  Unfortunately there
are such a wide array of text editors and IDEs that we can't possibly cover them all
in this documentation.

The exact command structure you will need differs based on how complex your project
will be.  As your project expands, we recommend moving to a Makefile-based or similar
system that allows you to specify simply the input source files.  In the future we will
be providing a Makefile include that you can use to switch Make over to using the 
toolchain instead of GCC.

Simple Projects
------------------

For simple, single C file projects like the one that will be covered in this tutorial,
the following series of commands work well:

**On Windows:**

.. code-block:: bat

    cd %~dp0
    dtcc %1 -o - | dtasm -i -o tmp.dobj16 -
    dtld -o tmp.dcpu16 tmp.dobj16
    dtemu tmp.dcpu16
    del tmp.dobj16
    del tmp.dcpu16

**On Linux:**
    
.. code-block:: bash

    #!/bin/bash
    if [ "$1" == "" ]; then
        exit 1
    fi
    dir=$(dirname $0)
    file="`cd "$(dirname $1)"; pwd`/$(basename $1)"
    pushd $dir
    ./dtcc "$file" -o - | ./dtasm -i -o tmp.dobj16 -
    ./dtld -o tmp.dcpu16 tmp.dobj16
    ./dtemu tmp.dcpu16
    rm tmp.dobj16 tmp.dcpu16
    popd

You can then pass the full path to the C file you want to compile to these scripts and
they will compile, assemble, link and then emulate the result for you.  If you don't
wish to automatically emulate the result, you can remove the calls to `dtemu`.  If this
is the case, you probably also want to remove references to `tmp.dcpu16` being deleted
since by default it's erased in these scripts.

As always on Linux, remember to make the script executable with `chmod u+x <file>`.