# DCPU-16 Toolchain

## Binaries
You can download binaries for Windows from the main http://dcputoolcha.in/ website.

## Source

### Important Software!

You will need:

* Bison
* Flex
* CMake
* _optionally_ Cygwin

### Instructions for Windows with Cygwin!

_Cygwin is only used for the initial project and code generation; you should build your Windows executables in Visual Studio 2010._

You need to install Bison and Flex.  You can do this by downloading either the GnuWin32 versions or by using the Cygwin versions if you're planning on running CMake from a Cygwin terminal.  You can download Bison and Flex from these links:

* http://gnuwin32.sourceforge.net/packages/flex.htm
* http://gnuwin32.sourceforge.net/packages/bison.htm

You will also need CMake to generate the build files, however you can not use the Cygwin version as this does not contain support for generating Visual Studio projects.  Thus, you must use the Windows installer from the CMake website; you can download CMake from:

* http://www.cmake.org/cmake/resources/software.html

**Super important note!**  If you are using Cygwin and you want to run CMake from a Cygwin terminal, ensure that you have not previously installed a Cygwin-version of CMake as this will override the native version of CMake even after we perform the next step.

You now need to add the non-Cygwin version of CMake to your system PATH.  CMake does offer to do this for you in the installer, but occasionally this will fail and you will need to do it yourself.

Once CMake is installed, start Cygwin terminal (you must open a new terminal since you modified PATH) and type the following commands:

```
> cd path/to/repo
> cmake -G "Visual Studio 10"
```

If this fails with an error of "No Such Generator" then you are not using the native version of CMake.  Uninstall the Cygwin version of CMake and try again.

If this fails with "unable to set key 'TMP', key 'Tmp' already exists" or something similar when configuring the C compiler, then you must restart your computer for this error to go away.  There doesn't seem to be another way around this issue.

Otherwise, this should work correctly and produce a Visual Studio 2010 solution that you can open up in Visual Studio 2010 C++ Express and compile!

### Instructions for Windows without Cygwin!

_This is not the supported method of building on Windows; see above to do it the recommended way with Cygwin._

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

If this fails with "unable to set key 'TMP', key 'Tmp' already exists" or something similar when configuring the C compiler, then you must restart your computer for this error to go away.  There doesn't seem to be another way around this issue.

Otherwise, this should work correctly and produce a Visual Studio 2010 solution that you can open up in Visual Studio 2010 C++ Express and compile!

### Instructions for Linux!

_Someone please add these; it has been done!_

### Instructions for Mac!

_Someone please add these!_