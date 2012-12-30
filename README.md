# DCPU-16 Toolchain

 *  Windows build status; ![Windows build status](http://dms.dcputoolcha.in/status/windows.png)
 *  Linux build status; ![Linux build status](http://dms.dcputoolcha.in/status/linux.png)
 *  Mac build status; ![Mac build status](http://dms.dcputoolcha.in/status/mac.png)

The build server and it's waterfall status can be accessed from http://bb.dcputoolcha.in:8080/waterfall.

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
* **and most importantly** you must initially run `cmake -G "Visual Studio 10"` from the command prompt.  Cygwin's case sensitivity causes the initial configuration and detection of the C compiler to fail.  You will need to terminate all instances of MSBuild.exe and run the configuration again from the command prompt if this occurs.
* Once you have done the initial configuration, you may then run cmake from Cygwin to update project files; it's just the initial configuration that can't be done in Cygwin.

### Instructions for Linux

#### Ubuntu 12.04

First install a few required packages with

```
$ sudo apt-get install bison flex cmake build-essential libreadline-dev libsdl1.2-dev libcurl4-openssl-dev 
```

Next clone the repository into a new folder

```
$ git clone git://github.com/DCPUTeam/DCPUToolchain.git dcputoolchain
$ cd dcputoolchain
```

Now generate the makefiles and and compile with

```
[dcputoolchain]$ cmake . && make -C libdcpu && make
```

If everything went well the binaries are in ```dcputoolchain/Debug```

#### Fedora 16
First install a few required packages with

**IMPORTANT NOTICE: Build Instructions outdated** libcurl dev headers need to be installed aswell

```
$ sudo yum install bison flex cmake mesa-lib* zlib-devel libpng-devel
$ sudo yum groupinstall "Development Tools"
```

Next clone the repository into a new folder

```
$ git clone git://github.com/DCPUTeam/DCPUToolchain.git dcputoolchain
$ cd dcputoolchain
```

Now generate the makefiles and and compile with

```
[dcputoolchain]$ cmake . && make -C libdcpu && make
```

If everything went well the binaries are in ```dcputoolchain/Debug```

### Instructions for Mac

First install Xcode which should install everything you need to compile.

Next, you'll need to install a few other things, I'll be using homebrew for this, which you can download [here](https://github.com/mxcl/homebrew/wiki/installation), but you could use port, fink or any other way you want to install it.

You will need to install cmake, git and sdl.

**IMPORTANT NOTICE: Build Instructions outdated** libcurl dev headers need to be installed aswell

```
$ brew install cmake
$ brew install git
$ brew install sdl
```

Now that you've installed git, you should be able to clone DCPUToolchain if you haven't already.

```
$ git clone https://github.com/DCPUTeam/DCPUToolchain.git dcputoolchain
$ cd dcputoolchain
```

Now we can build it.

```
[dcputoolchain]$ cmake . && make -C libdcpu && make
```

If everything looks like it didn't crash and burn, then you should find everything you need in ```dcputoolchain/Debug```

### Instructions for Emscripten

We have dropped support for Emscripten builds.  As the toolchain increases in functionality, having full Emscripten support was
no longer practical.

