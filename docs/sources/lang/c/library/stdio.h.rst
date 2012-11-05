:mod:`stdio.h` --- Input / Output
=============================================

.. module:: stdio.h
    :synopsis: Input and output operations.

The :mod:`stdio.h` header provides file access routines.

It provides a subset of functionality available in the standard C library
that appears on other platforms.

.. note::

    This documentation is not yet complete.  See http://www.cplusplus.com/reference/clibrary/cstdio/
    for a full list of available functions.
        
.. c:function:: int sprintf(char * str, const char * format, ...)
    
    Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, the content is stored as a C string in the buffer pointed by `str`. The size of the buffer should be large enough to contain the entire resulting string. A terminating null character is automatically appended after the content. After the `format` parameter, the function expects at least as many additional arguments as needed for format.