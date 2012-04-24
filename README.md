# DCPU-16 Toolchain

## Binaries
You can download binaries for Windows from the main http://dcputoolcha.in/ website.

## Source

### Instructions for Windows

You need to install Bison and Flex.  You can do this by downloading the GnuWin32 versions from these links:

* http://gnuwin32.sourceforge.net/packages/flex.htm
* http://gnuwin32.sourceforge.net/packages/bison.htm

Install these packages into a location that does not contain spaces in the path, such as C:\UNIX\GnuWin32.  Once installed, you must then add "C:\UNIX\GnuWin32\bin" to your system PATH environment variable (Google has the answer if you don't know how to do this).

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

If you are using Cygwin, then there are some important notes:

* You must use the GnuWin32 versions of Flex and Bison.
* You must use the Windows version of CMake.
* You can't have any of these tools installed as part of Cygwin.
* **and most importantly** you must initially run `cmake -G "Visual Studio 10"` from the command prompt.  Cygwin's case sensitivity causes the initial configuration and detection of the C compiler to screw up _badly_ and the only way to fix it is to _restart your computer_. You will get errors like "unable to set key 'TMP', key 'Tmp' already exists" which will also affect Visual Studio until you restart if you don't adhere to this!
* Once you have done the initial configuration, you may then run cmake from Cygwin to update project files; it's just the initial configuration that can't be done in Cygwin.

### Instructions for Linux

#### Ubuntu 12.04

First install a few required packages with

```
$ sudo apt-get install bison flex cmake build-essential
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
