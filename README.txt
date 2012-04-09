Building DCPU-16 Tools
================================

  DCPU-16 Tools uses Flex and Bison to generate the lexer
  and parser components of the assembler and compiler.  You
  need to have those tools installed in C:\UNIX\GnuWin32
  (the setup programs will default to Program Files, but since
  the tools are unix based, they don't like spaces).

  You can download Flex from:

	http://gnuwin32.sourceforge.net/packages/flex.htm

  You can download Bison from:

    http://gnuwin32.sourceforge.net/packages/bison.htm

  You then need to add (note the \bin subdirectory):

	C:\UNIX\GnuWin32\bin

  to your PATH variable.