:mod:`bstring.h` --- String Functions
=============================================

.. module:: bstring.h
    :synopsis: Manipulate strings in a safe manner.

The :mod:`bstring.h` header defines functions used to create, manipulate
and free strings as needed.

This library is a port of `The Better String Library <http://bstring.sourceforge.net/>`_ and
thus is made available under a BSD license.  See the `attached BSD copyright information <http://bstring.cvs.sourceforge.net/viewvc/bstring/tree/license.txt?pathrev=HEAD>`_
regarding the string library.

All of the documentation that follows is translated from `The Better String Library documentation <http://bstring.cvs.sourceforge.net/viewvc/bstring/tree/bstrlib.txt?pathrev=HEAD>`_, so credit
goes to Paul Hsieh for writing all of the description of functionality in this section.

    .. warning::

        All string literals compiled by the C compiler are embedded in the result as
        `const_bstring`.  There are no `const char*` literals in the C compiler.

    .. note::

        The C compiler will automatically expand `str[expr]` into `str->data[expr]` when
        working with bstrings.

Basics
----------------------------

.. c:type:: bstring

    Represents a better string.

.. c:type:: const_bstring

    Represents a constant, read-only better string.  All string literals are embedded as :c:type:`const_bstring`
    in the resulting code.

.. c:macro:: BSTR_OK

    Indicates success.

.. c:macro:: BSTR_ERR

    Indicates an error.

.. c:function:: bstring bstrcpy(const_bstring b1)

    Make a copy of the passed in bstring.  The copied bstring is returned if 
    there is no error, otherwise :c:macro:`NULL` is returned.

.. c:function:: int bassign(bstring a, const_bstring b)

    Overwrite the :c:type:`bstring` a with the contents of bstring b.  Note that the 
    bstring a must be a well defined and writable bstring.  If an error 
    occurs :c:macro:`BSTR_ERR` is returned and a is not overwritten.

.. c:function:: int bdestroy(bstring b)

    Deallocate the bstring passed.  Passing :c:macro:`NULL` in as a parameter will have 
    no effect.  Note that both the header and the data portion of the bstring 
    will be freed.  No other bstring function which modifies one of its 
    parameters will free or reallocate the header.  Because of this, in 
    general, bdestroy cannot be called on any declared struct tagbstring even 
    if it is not write protected.  A bstring which is write protected cannot 
    be destroyed via the bdestroy call.  Any attempt to do so will result in 
    no action taken, and :c:macro:`BSTR_ERR` will be returned.

.. c:function:: bstring bmidstr(const_bstring b, int left, int len)

    Create a bstring which is the substring of b starting from position left 
    and running for a length len (clamped by the end of the bstring b.)  If 
    there was no error, the value of this constructed bstring is returned 
    otherwise :c:macro:`NULL` is returned.

.. c:function:: int bassignmidstr(bstring a, const_bstring b, int left, int len)

    Overwrite the bstring a with the middle of contents of bstring b 
    starting from position left and running for a length len.  left and 
    len are clamped to the ends of b as with the function bmidstr.  Note that 
    the bstring a must be a well defined and writable bstring.  If an error 
    occurs :c:macro:`BSTR_ERR` is returned and a is not overwritten.

Manual Allocation
----------------------------

.. c:function:: int balloc(bstring b, int length)

    Increase the allocated memory backing the data buffer for the bstring b
    to a length of at least length.  If the memory backing the bstring b is
    already large enough, not action is performed.  This has no effect on the 
    bstring b that is visible to the bstring API.  Usually this function will 
    only be used when a minimum buffer size is required coupled with a direct 
    access to the ->data member of the bstring structure.

    Be warned that like any other bstring function, the bstring must be well
    defined upon entry to this function.  I.e., doing something like:

    .. code-block:: c

        b->slen *= 2; /* ?? Most likely incorrect */
        balloc (b, b->slen);

    is invalid, and should be implemented as:
    
    .. code-block:: c

        int t;
        if (BSTR_OK == balloc (b, t = (b->slen * 2))) b->slen = t;

    This function will return with :c:macro:`BSTR_ERR` if b is not detected as a valid 
    bstring or length is not greater than 0, otherwise :c:macro:`BSTR_OK` is returned.

.. c:function:: int ballocmin(bstring b, int length)

    Change the amount of memory backing the bstring b to at least length.  
    This operation will never truncate the bstring data including the 
    extra terminating '\0' and thus will not decrease the length to less than
    b->slen + 1.  Note that repeated use of this function may cause 
    performance problems (realloc may be called on the bstring more than
    the O(log(INT_MAX)) times).  This function will return with :c:macro:`BSTR_ERR` if b 
    is not detected as a valid bstring or length is not greater than 0, 
    otherwise :c:macro:`BSTR_OK` is returned.

    So for example:

    .. code-block:: c

        if (BSTR_OK == ballocmin (b, 64)) b->data[63] = 'x';

    The idea is that this will set the 64th character of b to 'x' if it is at 
    least 64 characters long otherwise do nothing.  And we know this is well 
    defined so long as the ballocmin call was successfully, since it will 
    ensure that b has been allocated with at least 64 characters.

.. c:function:: int btrunc(bstring b, int n)

    Truncate the bstring to at most n characters.  This function will return 
    with :c:macro:`BSTR_ERR` if b is not detected as a valid bstring or n is less than 
    0, otherwise :c:macro:`BSTR_OK` is returned.

Concatenation
----------------------------

.. c:function:: int bconcat(bstring b0, const_bstring b1)

    Concatenate the bstring b1 to the end of bstring b0.  The value :c:macro:`BSTR_OK` 
    is returned if the operation is successful, otherwise :c:macro:`BSTR_ERR` is 
    returned.

.. c:function:: int bconchar(bstring b, char c)

    Concatenate the character c to the end of bstring b.  The value :c:macro:`BSTR_OK` 
    is returned if the operation is successful, otherwise :c:macro:`BSTR_ERR` is 
    returned.

Comparisons
----------------------------

.. c:function:: int biseq(const_bstring b0, const_bstring b1)

    Compare the bstring b0 and b1 for equality.  If the bstrings differ, 0 
    is returned, if the bstrings are the same, 1 is returned, if there is an 
    error, -1 is returned.  If the length of the bstrings are different, this 
    function has O(1) complexity.  Contained '\0' characters are not treated 
    as a termination character.

    Note that the semantics of biseq are not completely compatible with 
    bstrcmp because of its different treatment of the '\0' character.

.. c:function:: int biseqcaseless(const_bstring b0, const_bstring b1)

    Compare two bstrings for equality without differentiating between case.  
    If the bstrings differ other than in case, 0 is returned, if the bstrings 
    are the same, 1 is returned, if there is an error, -1 is returned.  If 
    the length of the bstrings are different, this function is O(1).  '\0' 
    termination characters are not treated in any special way.

.. c:function:: int bstrcmp(const_bstring b0, const_bstring b1)

    Compare the bstrings b0 and b1 for ordering.  If there is an error, 
    SHRT_MIN is returned, otherwise a value less than or greater than zero, 
    indicating that the bstring pointed to by b0 is lexicographically less 
    than or greater than the bstring pointed to by b1 is returned.  If the
    bstring lengths are unequal but the characters up until the length of the 
    shorter are equal then a value less than, or greater than zero, 
    indicating that the bstring pointed to by b0 is shorter or longer than the 
    bstring pointed to by b1 is returned.  0 is returned if and only if the 
    two bstrings are the same.  If the length of the bstrings are different, 
    this function is O(n).  Like its standard C library counter part, the 
    comparison does not proceed past any '\0' termination characters 
    encountered.

    The seemingly odd error return value, merely provides slightly more 
    granularity than the undefined situation given in the C library function 
    strcmp.  The function otherwise behaves very much like strcmp().

    Note that the semantics of bstrcmp are not completely compatible with 
    biseq because of its different treatment of the '\0' termination 
    character.

.. c:function:: int bstrncmp(const_bstring b0, const_bstring b1, int n)

    Compare the bstrings b0 and b1 for ordering for at most n characters.  If 
    there is an error, SHRT_MIN is returned, otherwise a value is returned as 
    if b0 and b1 were first truncated to at most n characters then bstrcmp 
    was called with these new bstrings are paremeters.  If the length of the 
    bstrings are different, this function is O(n).  Like its standard C 
    library counter part, the comparison does not proceed past any '\0' 
    termination characters encountered.

    The seemingly odd error return value, merely provides slightly more 
    granularity than the undefined situation given in the C library function 
    strncmp.  The function otherwise behaves very much like strncmp().

.. c:function:: int bstricmp(const_bstring b0, const_bstring b1)

    Compare two bstrings without differentiating between case.  The return 
    value is the difference of the values of the characters where the two 
    bstrings first differ, otherwise 0 is returned indicating that the 
    bstrings are equal.  If the lengths are different, then a difference from 
    0 is given, but if the first extra character is '\0', then it is taken to 
    be the value UCHAR_MAX+1.

.. c:function:: int bstrnicmp(const_bstring b0, const_bstring b1, int n)

    Compare two bstrings without differentiating between case for at most n
    characters.  If the position where the two bstrings first differ is
    before the nth position, the return value is the difference of the values
    of the characters, otherwise 0 is returned.  If the lengths are different
    and less than n characters, then a difference from 0 is given, but if the 
    first extra character is '\0', then it is taken to be the value 
    UCHAR_MAX+1.
    
Find and Replace
----------------------------

.. c:function:: int binstr(const_bstring s1, int pos, const_bstring s2)

    Search for the bstring s2 in s1 starting at position pos and looking in a
    forward (increasing) direction.  If it is found then it returns with the 
    first position after pos where it is found, otherwise it returns :c:macro:`BSTR_ERR`.  
    The algorithm used is brute force; O(m*n).

.. c:function:: int binstrr(const_bstring s1, int pos, const_bstring s2)

    Search for the bstring s2 in s1 starting at position pos and looking in a
    backward (decreasing) direction.  If it is found then it returns with the 
    first position after pos where it is found, otherwise return :c:macro:`BSTR_ERR`.  
    Note that the current position at pos is tested as well -- so to be 
    disjoint from a previous forward search it is recommended that the 
    position be backed up (decremented) by one position.  The algorithm used 
    is brute force; O(m*n).

.. c:function:: int binstrcaseless(const_bstring s1, int pos, const_bstring s2)

    Search for the bstring s2 in s1 starting at position pos and looking in a
    forward (increasing) direction but without regard to case.  If it is 
    found then it returns with the first position after pos where it is 
    found, otherwise it returns :c:macro:`BSTR_ERR`. The algorithm used is brute force; 
    O(m*n).

.. c:function:: int binstrrcaseless(const_bstring s1, int pos, const_bstring s2)

    Search for the bstring s2 in s1 starting at position pos and looking in a
    backward (decreasing) direction but without regard to case.  If it is 
    found then it returns with the first position after pos where it is 
    found, otherwise return :c:macro:`BSTR_ERR`. Note that the current position at pos 
    is tested as well -- so to be disjoint from a previous forward search it 
    is recommended that the position be backed up (decremented) by one 
    position.  The algorithm used is brute force; O(m*n).

.. c:function:: int binchr(const_bstring b0, int pos, const_bstring b1)

    Search for the first position in b0 starting from pos or after, in which 
    one of the characters in b1 is found.  This function has an execution 
    time of O(b0->slen + b1->slen).  If such a position does not exist in b0, 
    then :c:macro:`BSTR_ERR` is returned.

.. c:function:: int binchrr(const_bstring b0, int pos, const_bstring b1)

    Search for the last position in b0 no greater than pos, in which one of 
    the characters in b1 is found.  This function has an execution time
    of O(b0->slen + b1->slen).  If such a position does not exist in b0, 
    then :c:macro:`BSTR_ERR` is returned.

.. c:function:: int bninchr(const_bstring b0, int pos, const_bstring b1)

    Search for the first position in b0 starting from pos or after, in which 
    none of the characters in b1 is found and return it.  This function has 
    an execution time of O(b0->slen + b1->slen).  If such a position does 
    not exist in b0, then :c:macro:`BSTR_ERR` is returned.

.. c:function:: int bninchrr(const_bstring b0, int pos, const_bstring b1)
  
    Search for the last position in b0 no greater than pos, in which none of 
    the characters in b1 is found and return it.  This function has an 
    execution time of O(b0->slen + b1->slen).  If such a position does not 
    exist in b0, then :c:macro:`BSTR_ERR` is returned.

.. c:function:: int bstrchr(const_bstring b, int c)

    Search for the character c in the bstring b forwards from the start of 
    the bstring.  Returns the position of the found character or :c:macro:`BSTR_ERR` if 
    it is not found.

    .. note::

        This has been implemented as a macro on top of bstrchrp().

.. c:function:: int bstrrchr(const_bstring b, int c)

    Search for the character c in the bstring b backwards from the end of the 
    bstring.  Returns the position of the found character or :c:macro:`BSTR_ERR` if it is 
    not found.

    .. note::

        This has been implemented as a macro on top of bstrrchrp().

.. c:function:: int bstrchrp(const_bstring b, int c, int pos)
  
    Search for the character c in b forwards from the position pos 
    (inclusive).  Returns the position of the found character or :c:macro:`BSTR_ERR` if 
    it is not found.

.. c:function:: int bstrrchrp(const_bstring b, int c, int pos)

    Search for the character c in b backwards from the position pos in bstring 
    (inclusive).  Returns the position of the found character or :c:macro:`BSTR_ERR` if 
    it is not found.

.. c:function:: int bsetstr(bstring b0, int pos, const_bstring b1, unsigned char fill)

    Overwrite the bstring b0 starting at position pos with the bstring b1. If 
    the position pos is past the end of b0, then the character "fill" is 
    appended as necessary to make up the gap between the end of b0 and pos.
    If b1 is :c:macro:`NULL`, it behaves as if it were a 0-length bstring. The value 
    :c:macro:`BSTR_OK` is returned if the operation is successful, otherwise :c:macro:`BSTR_ERR` is 
    returned.

.. c:function:: int binsert(bstring s1, int pos, const_bstring s2, unsigned char fill)

    Inserts the bstring s2 into s1 at position pos.  If the position pos is 
    past the end of s1, then the character "fill" is appended as necessary to 
    make up the gap between the end of s1 and pos.  The value :c:macro:`BSTR_OK` is 
    returned if the operation is successful, otherwise :c:macro:`BSTR_ERR` is returned.

.. c:function:: int binsertch(bstring s1, int pos, int len, unsigned char fill)

    Inserts the character fill repeatedly into s1 at position pos for a 
    length len.  If the position pos is past the end of s1, then the 
    character "fill" is appended as necessary to make up the gap between the 
    end of s1 and the position pos + len (exclusive).  The value :c:macro:`BSTR_OK` is 
    returned if the operation is successful, otherwise :c:macro:`BSTR_ERR` is returned.

.. c:function:: int bdelete(bstring s1, int pos, int len)

    Removes characters from pos to pos+len-1 and shifts the tail of the 
    bstring starting from pos+len to pos.  len must be positive for this call 
    to have any effect.  The section of the bstring described by (pos, len) 
    is clamped to boundaries of the bstring b.  The value :c:macro:`BSTR_OK` is returned 
    if the operation is successful, otherwise :c:macro:`BSTR_ERR` is returned.

.. c:function:: int breplace(bstring b1, int pos, int len, const_bstring b2, unsigned char fill)

    Replace a section of a bstring from pos for a length len with the bstring 
    b2. If the position pos is past the end of b1 then the character "fill"
    is appended as necessary to make up the gap between the end of b1 and 
    pos.

.. c:function:: int bfindreplace(bstring b, const_bstring find, const_bstring replace, int position)

    Replace all occurrences of the find substring with a replace bstring 
    after a given position in the bstring b.  The find bstring must have a 
    length > 0 otherwise :c:macro:`BSTR_ERR` is returned.  This function does not 
    perform recursive per character replacement; that is to say successive
    searches resume at the position after the last replace.

    So for example:
    
    .. code-block:: c

        bfindreplace (a0 = bfromcstr("aabaAb"), a1 = bfromcstr("a"),
                      a2 = bfromcstr("aa"), 0);

    Should result in changing a0 to "aaaabaaAb".

    This function performs exactly (b->slen - position) bstring comparisons, 
    and data movement is bounded above by character volume equivalent to size 
    of the output bstring.

.. c:function:: int bfindreplacecaseless(bstring b, const_bstring find, const_bstring replace, int position)

    Replace all occurrences of the find substring, ignoring case, with a 
    replace bstring after a given position in the bstring b.  The find bstring 
    must have a length > 0 otherwise :c:macro:`BSTR_ERR` is returned.  This function 
    does not perform recursive per character replacement; that is to say 
    successive searches resume at the position after the last replace.

    So for example:
    
    .. code-block:: c

        bfindreplacecaseless (a0 = bfromcstr("AAbaAb"), a1 = bfromcstr("a"),
                              a2 = bfromcstr("aa"), 0);

    Should result in changing a0 to "aaaabaaaab".

    This function performs exactly (b->slen - position) bstring comparisons, 
    and data movement is bounded above by character volume equivalent to size 
    of the output bstring.

Utilities
---------------------

.. c:function:: int bpattern(bstring b, int len)

    Replicate the starting bstring, b, end to end repeatedly until it 
    surpasses len characters, then chop the result to exactly len characters. 
    This function operates in-place.  This function will return with :c:macro:`BSTR_ERR` 
    if b is :c:macro:`NULL` or of length 0, otherwise :c:macro:`BSTR_OK` is returned.

.. c:function:: int btoupper(bstring b)

    Convert contents of bstring to upper case.  This function will return with 
    :c:macro:`BSTR_ERR` if b is :c:macro:`NULL` or of length 0, otherwise :c:macro:`BSTR_OK` is returned.

.. c:function:: int btolower(bstring b)

    Convert contents of bstring to lower case.  This function will return with 
    :c:macro:`BSTR_ERR` if b is :c:macro:`NULL` or of length 0, otherwise :c:macro:`BSTR_OK` is returned.

.. c:function:: int bltrimws(bstring b)

    Delete whitespace contiguous from the left end of the bstring.  This 
    function will return with :c:macro:`BSTR_ERR` if b is :c:macro:`NULL` or of length 0, otherwise 
    :c:macro:`BSTR_OK` is returned.

.. c:function:: int brtrimws(bstring b)

    Delete whitespace contiguous from the right end of the bstring.  This 
    function will return with :c:macro:`BSTR_ERR` if b is :c:macro:`NULL` or of length 0, otherwise 
    :c:macro:`BSTR_OK` is returned.

.. c:function:: int btrimws(bstring b)

    Delete whitespace contiguous from both ends of the bstring.  This function 
    will return with :c:macro:`BSTR_ERR` if b is :c:macro:`NULL` or of length 0, otherwise :c:macro:`BSTR_OK` 
    is returned.