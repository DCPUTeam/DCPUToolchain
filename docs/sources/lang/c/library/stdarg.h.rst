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
    
.. c:macro:: void va_start(va_list list, paramN)
    
    Initializes the object `list` of type va_list based on the variable arguments passed to this macro.  `paramN`
    is the name of the last named parameter before the variable arguments begin.
    
.. c:macro:: type va_arg(va_list list, type)
    
    Returns the next variable argument in the list `list` as an expression of type `type`.
    Consecutive calls to this function will return the extra arguments to this function
    in the order they were passed.
    
    .. warning::
        
        Notice: va_arg cannot determine the actual type of the argument passed to the function, but will use the given type `type`.
        
    .. warning::
        
        Notice: va_arg does not detect the last parameter passed to the function, nor does it recorgnize if it is trying to access more
        arguments than were passed. The function should be designed in a way, so that the correct amount of calls to va_args can be
        deduced from the fixed arguments given to the function (i.e. none of the variable argument list).
    
.. c:macro:: void va_end(va_list list)
    
    Performs clean up actions, that are needed so that the function can return normally.
    This macro should be executed before the function returns whenever va_start has been previously used in that function.
