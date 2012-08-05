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
$ sudo apt-get install bison flex cmake build-essential libreadline-dev libsdl1.2-dev libcurl4-openssl-dev
```

Next clone the repository into a new folder

```
$ git clone git://github.com/DCPUTeam/DCPUToolchain.git dcputoolchain
$ cd dcputoolchain
```

Now generate the makefiles and and compile with

```
dcputoolchain$ cd build
dcputoolchain/build$ cmake ..
dcputoolchain/build$ make
```

If everything went well the binaries are in ```dcputoolchain/Debug```

#### Fedora 16
First install a few required packages with

**IMPORTANT NOTICE: Build Instructions outdated** libcurl dev headers need to be
installed aswell
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
dcputoolchain$ cd build
dcputoolchain/build$ cmake ..
dcputoolchain/build$ make
```

If everything went well the binaries are in ```dcputoolchain/Debug```

### Instructions for Mac

First install Xcode which should install everything you need to compile.

Next, you'll need to install a few other things, I'll be using homebrew for this, which you can download [here](https://github.com/mxcl/homebrew/wiki/installation), but you could use port, fink or any other way you want to install it.

You will need to install cmake, git, hg and sdl.

**IMPORTANT NOTICE: Build Instructions outdated** libcurl dev headers need to be
installed aswell
```
$ brew install cmake
$ brew install git
$ brew install hg
$ brew install sdl
```

Once all of these are installed, you're first going to want to clone the libtcod source from [here](https://bitbucket.org/jice/libtcod).  The reason we are building it from scratch, is because the downloads only has a i386 built dylib, and most likely your going to be building with x64_86, so we'll just build manually.

Clone the repo to a folder on your computer, and then enter the directory.

```
$ hg clone https://bitbucket.org/jice/libtcod
$ cd libtcod
```

Now we can build libtcod with cmake and make.

```
$ cmake .
$ make
```

You may see a few warnings, as long as it says 100% at the end, you should be just fine.  In order to create the dylib you need, you'll need to use make install, and then copy it to your lib folder, I use /usr/local/lib, but you can use /usr/lib if that's what you use/prefer.

```
$ make install
$ sudo cp libtcod.dylib /usr/local/lib
$ sudo cp -r include /usr/include/libtcod
```

Now that you've copied the dylib for libtcod, you should be able to clone DCPUToolchain if you haven't already.

```
$ git clone https://github.com/DCPUTeam/DCPUToolchain.git
$ cd DCPUToolchain/build
```

Now we can build it.

```
$ cmake ..
$ make
```

If everything looks like it didn't crash and burn, then you should find everything you need in ```dcputoolchain/Debug```

### Instructions for Emscripten

The toolchain has experimental and (currently) limited support for Emscripten.  Ensure you have recursively updated all submodules
in the repository and then follow the instructions below.

```
$ mkdir build-emcc
$ cd build-emcc
$ cmake -C ../html5/emscripten/Emscripten.cmake .. 
$ make dtemu
```

Note that currently only building `dtemu` works due to a conflict that exists when building the preprocessor parser inside the
static library.  We hope to have this issue sorted so that all tools can be built successfully.

#### Testing with Emscripten

In order to run the toolchain under Emscripten, you must be running it from a web server (due to cross-HTTP request restrictions).  To run the
toolchain, start a web server local to the _build directory_ (in the case above, this is in the `build-emcc` folder).  For example,
if you have Python installed, you can easily do:

```
$ cd build-emcc
$ python -m SimpleHTTPServer 8888
```

Then navigate your web browser to http://localhost:8888/.

