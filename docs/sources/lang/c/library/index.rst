C Standard Library
=============================================

We haven't yet finishing documenting or designing this!

What we can say is there will be two elements to the standard library:

  * The standard library
  * The compatibility layer
  
The compatibility layer will provide the same C functions that are available to applications compiled
for normal computers.  The reason that we're relegating this as a compatibility layer and instead
modelling our own standard C library is for security.  Quite a few of the standard ANSI C functions
are not secure (susceptable to buffer overflows) and quite a few POSIX functions don't make sense
on the DCPU at all.