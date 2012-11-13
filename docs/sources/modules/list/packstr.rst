.. _module-packstr:

Packed Strings
===================

This preprocessor module provides a string packing directive, which will
compress a given string such that there are two characters stored in
every word.

.PACKSTR "string"
-------------------
This directive outputs a series of DAT instructions with the correct
packing values.