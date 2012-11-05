:mod:`stdarg.h` --- Variable Arguments
=============================================

.. module:: stdarg.h
    :synopsis: Access variable function arguments.

The :mod:`stdarg.h` header defines functions used to access variable arguments
to C functions.

It provides a subset of functionality available in the standard C library
that appears on other platforms.

.. note::

    This documentation is not yet complete.  See http://www.cplusplus.com/reference/clibrary/cstdarg/
    for a full list of available functions.
        
.. c:type:: va_list
    
    A structure representing a variable argument list.
    
.. c:macro:: void va_start(list, param)
    
    Initializes the `list` structure based on the variable arguments passed to this macro.  `param`
    is the name of the last named parameter before the variable arguments begin.
    
.. c:macro:: void va_arg(list, type)
    
    Returns the next variable argument in the list `list`, assuming it is of type `type`.
    
.. c:macro:: void va_end(list)
    
    Frees any memory associated with the variable argument list `list`.