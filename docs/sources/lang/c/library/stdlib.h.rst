:mod:`stdlib.h` --- General Functions
=============================================

.. module:: stdlib.h
    :synopsis: General functionality.

The :mod:`stdlib.h` header defines general C functionality.

It provides a subset of functionality available in the standard C library
that appears on other platforms.

.. note::

    This documentation is not yet complete.  See http://www.cplusplus.com/reference/clibrary/cstdlib/
    for a full list of available functions.

.. c:function:: int atoi(const char* str)

    Converts the numeric value in `str` to an integer representation.
        
.. c:function:: char* itoa(int value, char * str, int base)
    
    Converts the numeric value in `value` to a string represented in the specified base.

.. c:function:: void* calloc(size_t num, size_t size)

    Dynamically allocates an array with a certain number of elements, with each element being of `size` bytes.

.. c:function:: void free(void* ptr)
    
    Frees previously allocated memory.
    
.. c:function:: void* malloc(size_t size)
    
    Dynamically allocates the specified number of bytes and returns a pointer to the block of memory.
    
.. c:function:: void* realloc(void* ptr, size_t size)
    
    Resizes a previously allocated block of memory, returning the pointer to the new block of memory.
    
    .. note::
        
        :c:func:`realloc()` may not always reallocate in place, so you must reassign the source of ptr with the
        result of :c:func:`realloc()`.
        
.. c:function:: void abort()
    
    Terminates the current process.
    
.. c:function:: int abs(int n)
    
    Returns the absolute value of a number.
    
.. c:type:: div_s
    
    A structure that represents the result of :c:func:`div()`.
    
.. c:function:: struct div_s div(int numerator, int denominator)
    
    Performs integer division returning a structure that holds both the result and the modulo of
    the division operation.
    
