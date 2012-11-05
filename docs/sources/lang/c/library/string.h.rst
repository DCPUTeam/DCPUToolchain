:mod:`string.h` --- String Functions
=============================================

.. module:: string.h
    :synopsis: Manipulate strings.

The :mod:`string.h` header defines functions used to create, manipulate
and free strings as needed.

It provides a subset of functionality available in the standard C library
that appears on other platforms.

.. note::

    This documentation is not yet complete.  See http://www.cplusplus.com/reference/clibrary/cstring/
    for a full list of available functions.

.. c:function:: void* memcpy(void* destination, const void* source, size_t num)

    Copies `num` bytes from `source` to `destination`. 

.. c:function:: void* memmove(void* destination, const void* source, size_t num)
    
    Similar to :c:func:`memcpy()` except that it uses an intermediate array that is
    dynamically allocated to ensure that if destination and source are the same,
    data will not become overlapped during the copy process.

.. c:function:: char* strcpy(char* destination, const char* source)
    
    Copies the `source` string into the `destination`.
    
    .. warning::
        
        `destination` must be large enough to hold `source`!
        
.. c:function:: char* strncpy(char* destination, const char* source, size_t num)

    Copies up to `num` characters from `source` into `destination`.
    
.. c:function:: char* strcat(char* destination, const char* source)

    Concatenates `source` string onto `destination`.
    
    .. warning::
        
        `destination` must be large enough to hold both the current value and `source`!
    
.. c:function:: char* strncat(char* destination, char* source, size_t num)

    Concatenates up to `num` characters from `source` string onto `destination`.
    
.. c:function:: int memcmp(const void* first, const void* second, size_t num)

    Performs a comparison of bytes from the `first` and `second` memory blocks, up to `num` bytes.
    Returns -1 if `first` is less than `second`, 1 if `first` is greater than `second` and 0
    if both `first` and `second` are identical.
        
.. c:function:: int strcmp(const char* first, const char* second)

    Compares two strings and returns a result indicating their relation to one another.  Returns
    -1 if `first` is less than `second`, 1 if `first` is greater than `second` and 0 if both `first`
    and `second` are identical.
    
    .. warning::
        
        `first` and `second` must both be properly terminated strings.
        
.. c:function:: int strncmp(const char* first, const char* second, size_t num)

    Compares up to `num` bytes in the `first` and `second` strings and returns their relation to
    each other.  Returns -1 if `first` is less than `second`, 1 if `first` is greater than `second`
    and 0 if both `first` and `second` are identical.   
    
.. c:function:: void* memset(void* destination, int value, size_t num)
    
    Sets the contents of `destination` such that each byte is set to `value` for `num` bytes.
    
.. c:function:: size_t strlen(const char* str)
    
    Returns the length of the string.
    
    .. warning::
        
        `str` must both a properly terminated string.
    
    