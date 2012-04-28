# DCPU-16 Toolchain

## Binaries
You can download binaries for Windows from the main http://dcputoolcha.in/ website.

## Source

### Instructions for Windows

**IMPORTANT NOTICE:** GnuWin32 no longer works as it is too out-of-date to support the reentrant lexer and parser used in the preprocesser system.  If you used the old instructions, you must now uninstall the GnuWin32 versions, remove GnuWin32 from your PATH and then follow the instructions below.

You need to install Cygwin.  You can do this by downloading Cygwin from this link:

* http://cygwin.com/install.html

Ensure that during the setup process you select the Flex and Bison packages during installation.  Do **not** install CMake under Cygwin.  You should install Cygwin into a path without spaces (it will inform you to do this).

Once installed, you must then add the Cygwin bin/ folder (located under where-ever you installed Cygwin) to your system PATH environment variable (Google has the answer if you don't know how to do this).  For example, I install Cygwin in C:\UNIX, thus the path for me to add would be C:\UNIX\bin.

You must then install CMake; you can download CMake from:

* http://www.cmake.org/cmake/resources/software.html

You now need to add CMake to your system PATH.  CMake does offer to do this for you in the installer, but occasionally this will fail and you will need to do it yourself.

Once CMake is installed, start a command prompt (you must open a new command prompt since you modified PATH) and type the following commands:

```
> cd path/to/repo
> cmake -G "Visual Studio 10"
```

Otherwise, this should work correctly and produce a Visual Studio 2010 solution that you can open up in Visual Studio 2010 C++ Express and compile!

#### Cygwin Notice

When using Cygwin, then there are some important notes:

* You must use the Windows version of CMake.  You can't have this installed as part of Cygwin.
* **and most importantly** you must initially run `cmake -G "Visual Studio 10"` from the command prompt.  Cygwin's case sensitivity causes the initial configuration and detection of the C compiler to screw up _badly_ and the only way to fix it is to _restart your computer_. You will get errors like "unable to set key 'TMP', key 'Tmp' already exists" which will also affect Visual Studio until you restart if you don't adhere to this!
* Once you have done the initial configuration, you may then run cmake from Cygwin to update project files; it's just the initial configuration that can't be done in Cygwin.

### Instructions for Linux

#### Ubuntu 12.04

First install a few required packages with

```
$ sudo apt-get install bison flex cmake build-essential libreadline-dev
```

Download [libtcod](http://doryen.eptalys.net/libtcod/download/) and copy

```
$ sudo cp libtcod-gui.so  libtcod.so  libtcodxx.so /usr/local/lib
$ sudo cp -R include /usr/local/include
```

If you want to compile libtcod yourself you need libsdl1.2-dev. Next create a directory and clone the repository into the src folder

```
$ mkdir -p dcputoolchain/build
dcputoolchain$ cd dcputoolchain
dcputoolchain$ git clone git@github.com:DCPUTeam/DCPUToolchain.git src
```

Now generate the makefiles and and compile with

```
dcputoolchain$ cd build
dcputoolchain/build$ cmake ../src/
dcputoolchain/build$ make
```

If everything went well the binaries are in ```dcputoolchain/src/Debug```. You need to copy bootstrap.asm into the Debug folder, otherwise the assembler won't assemble your project

```
~/dcputoolchain/src/Debug$ cp ../compiler/bootstrap.asm ./
```

### Instructions for Mac

_Someone please add these!_
