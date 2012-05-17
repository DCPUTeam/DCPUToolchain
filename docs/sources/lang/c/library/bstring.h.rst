:mod:`bstring.h` --- String Functions
=============================================

.. module:: bstring.h
    :synopsis: Manipulate strings in a safe manner.

The :mod:`bstring.h` header defines functions used to create, manipulate
and free strings as needed.

This library is `The Better String Library <http://bstring.sourceforge.net/>`_ and
thus is made available under a BSD license.  See the `attached BSD copyright information <http://bstring.cvs.sourceforge.net/viewvc/bstring/tree/license.txt?pathrev=HEAD>`_
regarding the string library.

All of the documentation that follows is translated from `The Better String Library documentation <http://bstring.cvs.sourceforge.net/viewvc/bstring/tree/bstrlib.txt?pathrev=HEAD>`_
and as such all of the descriptions are written by Paul Hsieh (the author of The Better String Library).

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

        bfindreplace (a0 = bstrcpy("aabaAb"), "a", "aa", 0);

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

        bfindreplacecaseless (a0 = bstrcpy("AAbaAb"), "a", "aa", 0);

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
    
String Lists
----------------------------

.. c:type:: struct bstrList

    Represents a list of strings.  The struct bstrList output structure is 
    declared as follows:

    .. code-block:: c
        
        struct bstrList {
            int qty, mlen;
            bstring * entry;
        };

.. c:function:: struct bstrList * bstrListCreate()

    Create an empty struct bstrList.

    The entry field actually is an array with qty number entries.  The mlen
    record counts the maximum number of bstring's for which there is memory
    in the entry record.

    The Bstrlib API does *NOT* include a comprehensive set of functions for 
    full management of struct bstrList in an abstracted way.  The reason for 
    this is because aliasing semantics of the list are best left to the user 
    of this function, and performance varies wildly depending on the 
    assumptions made.  For a complete list of bstring data type it is 
    recommended that the C++ public std::vector<CBString> be used, since its
    semantics are usage are more standard.

.. c:function:: int bstrListDestroy(struct bstrList * sl)

    Destroy a struct bstrList structure that was returned by the bsplit 
    function.  Note that this will destroy each bstring in the ->entry array
    as well.  See bstrListCreate() above for structure of struct bstrList.

.. c:function:: int bstrListAlloc(struct bstrList * sl, int msz)

    Ensure that there is memory for at least msz number of entries for the
    list.

.. c:function:: int bstrListAllocMin(struct bstrList * sl, int msz)

    Try to allocate the minimum amount of memory for the list to include at
    least msz entries or sl->qty whichever is greater.

Splitting and Joining
----------------------------

.. c:function:: struct bstrList * bsplit(bstring str, unsigned char splitChar)

    Create an array of sequential substrings from str divided by the 
    character splitChar.  Successive occurrences of the splitChar will be 
    divided by empty bstring entries, following the semantics from the Python 
    programming language.  To reclaim the memory from this output structure, 
    bstrListDestroy () should be called.  See bstrListCreate() above for 
    structure of struct bstrList.

.. c:function:: struct bstrList * bsplits(bstring str, const_bstring splitStr)

    Create an array of sequential substrings from str divided by any 
    character contained in splitStr.  An empty splitStr causes a single entry 
    bstrList containing a copy of str to be returned.  See bstrListCreate() 
    above for structure of struct bstrList.

.. c:function:: struct bstrList * bsplitstr(bstring str, const_bstring splitStr)

    Create an array of sequential substrings from str divided by the entire 
    substring splitStr.  An empty splitStr causes a single entry bstrList 
    containing a copy of str to be returned.  See bstrListCreate() above for 
    structure of struct bstrList.

.. c:function:: bstring bjoin(const struct bstrList * bl, const_bstring sep)

    Join the entries of a bstrList into one bstring by sequentially 
    concatenating them with the sep bstring in between.  If sep is :c:macro:`NULL`, it
    is treated as if it were the empty bstring.  Note that:
    
    .. code-block:: c
        
        bjoin (l = bsplit (b, s->data[0]), s);
    
    should result in a copy of b, if s->slen is 1.  If there is an error :c:macro:`NULL` 
    is returned, otherwise a bstring with the correct result is returned.  
    See bstrListCreate() above for structure of struct bstrList.

.. c:function:: int bsplitcb (const_bstring str, unsigned char splitChar, int pos, int (* cb) (void * parm, int ofs, int len), void * parm)

    Iterate the set of disjoint sequential substrings over str starting at 
    position pos divided by the character splitChar.  The parm passed to 
    bsplitcb is passed on to cb.  If the function cb returns a value < 0, 
    then further iterating is halted and this value is returned by bsplitcb.

    Note: Non-destructive modification of str from within the cb function 
    while performing this split is not undefined.  bsplitcb behaves in 
    sequential lock step with calls to cb.  I.e., after returning from a cb 
    that return a non-negative integer, bsplitcb continues from the position 
    1 character after the last detected split character and it will halt 
    immediately if the length of str falls below this point.  However, if the 
    cb function destroys str, then it *must* return with a negative value, 
    otherwise bsplitcb will continue in an undefined manner.

    This function is provided as an incremental alternative to bsplit that is
    abortable and which does not impose additional memory allocation.

.. c:function:: int bsplitscb (const_bstring str, const_bstring splitStr, int pos, int (* cb) (void * parm, int ofs, int len), void * parm)

    Iterate the set of disjoint sequential substrings over str starting at 
    position pos divided by any of the characters in splitStr.  An empty 
    splitStr causes the whole str to be iterated once.  The parm passed to 
    bsplitcb is passed on to cb.  If the function cb returns a value < 0, 
    then further iterating is halted and this value is returned by bsplitcb.

    Note: Non-destructive modification of str from within the cb function 
    while performing this split is not undefined.  bsplitscb behaves in 
    sequential lock step with calls to cb.  I.e., after returning from a cb 
    that return a non-negative integer, bsplitscb continues from the position 
    1 character after the last detected split character and it will halt 
    immediately if the length of str falls below this point.  However, if the 
    cb function destroys str, then it *must* return with a negative value, 
    otherwise bsplitscb will continue in an undefined manner.

    This function is provided as an incremental alternative to bsplits that 
    is abortable and which does not impose additional memory allocation.

.. c:function:: int bsplitstrcb (const_bstring str, const_bstring splitStr, int pos, int (* cb) (void * parm, int ofs, int len), void * parm)

    Iterate the set of disjoint sequential substrings over str starting at 
    position pos divided by the entire substring splitStr.  An empty splitStr 
    causes each character of str to be iterated.  The parm passed to bsplitcb 
    is passed on to cb.  If the function cb returns a value < 0, then further 
    iterating is halted and this value is returned by bsplitcb.

    Note: Non-destructive modification of str from within the cb function 
    while performing this split is not undefined.  bsplitstrcb behaves in 
    sequential lock step with calls to cb.  I.e., after returning from a cb 
    that return a non-negative integer, bsplitstrcb continues from the position 
    1 character after the last detected split character and it will halt 
    immediately if the length of str falls below this point.  However, if the 
    cb function destroys str, then it *must* return with a negative value, 
    otherwise bsplitscb will continue in an undefined manner.

    This function is provided as an incremental alternative to bsplitstr that 
    is abortable and which does not impose additional memory allocation.

Formatting
----------------

.. warning::

    The signatures of these functions will change to accept `const_bstring` instead of
    `const char*`.

.. c:function:: bstring bformat(const char * fmt, ...)

    Takes the same parameters as printf (), but rather than outputting 
    results to stdio, it forms a bstring which contains what would have been 
    output. Note that if there is an early generation of a '\0' character, 
    the bstring will be truncated to this end point.

    Note that %s format tokens correspond to '\0' terminated char * buffers, 
    not bstrings.  To print a bstring, first dereference data element of the 
    the bstring:
    
    .. code-block:: c
        
        /* b1->data needs to be '\0' terminated, so tagbstrings generated 
           by blk2tbstr () might not be suitable. */
        b0 = bformat ("Hello, %s", b1->data);
    
    Note that if the BSTRLIB_NOVSNP macro has been set when bstrlib has been 
    compiled the bformat function is not present.

.. c:function:: int bformata(bstring b, const char * fmt, ...)

    In addition to the initial output buffer b, bformata takes the same 
    parameters as printf (), but rather than outputting results to stdio, it 
    appends the results to the initial bstring parameter. Note that if 
    there is an early generation of a '\0' character, the bstring will be 
    truncated to this end point.

    Note that %s format tokens correspond to '\0' terminated char * buffers, 
    not bstrings.  To print a bstring, first dereference data element of the 
    the bstring:
    
    .. code-block:: c
        
        /* b1->data needs to be '\0' terminated, so tagbstrings generated 
           by blk2tbstr () might not be suitable. */
        bformata (b0 = bfromcstr ("Hello"), ", %s", b1->data); 
    
    Note that if the BSTRLIB_NOVSNP macro has been set when bstrlib has been 
    compiled the bformata function is not present.

.. c:function:: int bassignformat(bstring b, const char * fmt, ...)

    After the first parameter, it takes the same parameters as printf (), but 
    rather than outputting results to stdio, it outputs the results to 
    the bstring parameter b. Note that if there is an early generation of a 
    '\0' character, the bstring will be truncated to this end point.

    Note that %s format tokens correspond to '\0' terminated char * buffers, 
    not bstrings.  To print a bstring, first dereference data element of the 
    the bstring:
    
    .. code-block:: c
        
        /* b1->data needs to be '\0' terminated, so tagbstrings generated 
           by blk2tbstr () might not be suitable. */
        bassignformat (b0 = bfromcstr ("Hello"), ", %s", b1->data); 
    
    Note that if the BSTRLIB_NOVSNP macro has been set when bstrlib has been 
    compiled the bassignformat function is not present.

.. c:function:: int bvcformata(bstring b, int count, const char * fmt, va_list arglist)

    The bvcformata function formats data under control of the format control 
    string fmt and attempts to append the result to b.  The fmt parameter is 
    the same as that of the printf function.  The variable argument list is 
    replaced with arglist, which has been initialized by the va_start macro.
    The size of the output is upper bounded by count.  If the required output
    exceeds count, the string b is not augmented with any contents and a value
    below :c:macro:`BSTR_ERR` is returned.  If a value below -count is returned then it
    is recommended that the negative of this value be used as an update to the
    count in a subsequent pass.  On other errors, such as running out of 
    memory, parameter errors or numeric wrap around :c:macro:`BSTR_ERR` is returned.  
    :c:macro:`BSTR_OK` is returned when the output is successfully generated and 
    appended to b.

    Note: There is no sanity checking of arglist, and this function is
    destructive of the contents of b from the b->slen point onward.  If there 
    is an early generation of a '\0' character, the bstring will be truncated 
    to this end point.

    Although this function is part of the external API for Bstrlib, the 
    interface and semantics (length limitations, and unusual return codes) 
    are fairly atypical.  The real purpose for this function is to provide an 
    engine for the bvformata macro.

    Note that if the BSTRLIB_NOVSNP macro has been set when bstrlib has been 
    compiled the bvcformata function is not present.

Streams
-------------

.. c:type:: size_t (*bNread)(void *buff, size_t elsize, size_t nelem, void *parm)

    A function which reads in a requested amount of characters.

.. c:type:: int (*bNgetc)(void * parm)

    A function which reads in a single character.

.. c:function:: bstring bread(bNread readPtr, void * parm)

    Read an entire stream into a bstring, verbatum.  The readPtr function 
    pointer is compatible with fread sematics, except that it need not obtain 
    the stream data from a file.  The intention is that parm would contain 
    the stream data context/state required (similar to the role of the FILE* 
    I/O stream parameter of fread.)

    Abstracting the block read function allows for block devices other than 
    file streams to be read if desired.  Note that there is an ANSI 
    compatibility issue if "fread" is used directly; see the ANSI issues 
    section below.

.. c:function:: int breada (bstring b, bNread readPtr, void * parm)

    Read an entire stream and append it to a bstring, verbatum.  Behaves 
    like bread, except that it appends it results to the bstring b.
    :c:macro:`BSTR_ERR` is returned on error, otherwise 0 is returned.

.. c:function:: bstring bgets(bNgetc getcPtr, void * parm, char terminator)

    Read a bstring from a stream.  As many bytes as is necessary are read
    until the terminator is consumed or no more characters are available from
    the stream.  If read from the stream, the terminator character will be
    appended to the end of the returned bstring.  The getcPtr function must 
    have the same semantics as the fgetc C library function (i.e., returning 
    an integer whose value is negative when there are no more characters 
    available, otherwise the value of the next available unsigned character 
    from the stream.)  The intention is that parm would contain the stream 
    data context/state required (similar to the role of the FILE* I/O stream 
    parameter of fgets.)  If no characters are read, or there is some other 
    detectable error, :c:macro:`NULL` is returned.

    bgets will never call the getcPtr function more often than necessary to
    construct its output (including a single call, if required, to determine 
    that the stream contains no more characters.)

    Abstracting the character stream function and terminator character allows
    for different stream devices and string formats other than '\n' 
    terminated lines in a file if desired (consider \032 terminated email 
    messages, in a UNIX mailbox for example.)

    For files, this function can be used analogously as fgets as follows:
    
    .. code-block:: c
        
        fp = fopen ( ... );
        if (fp) b = bgets ((bNgetc) fgetc, fp, '\n');

    (Note that only one terminator character can be used, and that '\0' is 
    not assumed to terminate the stream in addition to the terminator 
    character. This is consistent with the semantics of fgets.)

.. c:function:: int bgetsa(bstring b, bNgetc getcPtr, void * parm, char terminator)

    Read from a stream and concatenate to a bstring.  Behaves like bgets, 
    except that it appends it results to the bstring b.  The value 1 is 
    returned if no characters are read before a negative result is returned
    from getcPtr.  Otherwise :c:macro:`BSTR_ERR` is returned on error, and 0 is returned
    in other normal cases.

.. c:function:: int bassigngets(bstring b, bNgetc getcPtr, void * parm, char terminator)

    Read from a stream and concatenate to a bstring.  Behaves like bgets, 
    except that it assigns the results to the bstring b.  The value 1 is 
    returned if no characters are read before a negative result is returned
    from getcPtr.  Otherwise :c:macro:`BSTR_ERR` is returned on error, and 0 is returned
    in other normal cases.

.. c:function:: struct bStream * bsopen(bNread readPtr, void * parm)
  
    Wrap a given open stream (described by a fread compatible function 
    pointer and stream handle) into an open bStream suitable for the bstring 
    library streaming functions.
  
.. c:function:: void * bsclose(struct bStream * s)
  
    Close the bStream, and return the handle to the stream that was 
    originally used to open the given stream.  If s is :c:macro:`NULL` or detectably 
    invalid, :c:macro:`NULL` will be returned.

.. c:function:: int bsbufflength(struct bStream * s, int sz)

    Set the length of the buffer used by the bStream.  If sz is the macro
    BSTR_BS_BUFF_LENGTH_GET (which is 0), the length is not set.  If s is 
    :c:macro:`NULL` or sz is negative, the function will return with :c:macro:`BSTR_ERR`, otherwise 
    this function returns with the previous length.

.. c:function:: int bsreadln(bstring r, struct bStream * s, char terminator)
  
    Read a bstring terminated by the terminator character or the end of the
    stream from the bStream (s) and return it into the parameter r.  The 
    matched terminator, if found, appears at the end of the line read.  If
    the stream has been exhausted of all available data, before any can be
    read, :c:macro:`BSTR_ERR` is returned.  This function may read additional characters 
    into the stream buffer from the core stream that are not returned, but 
    will be retained for subsequent read operations.  When reading from high 
    speed streams, this function can perform significantly faster than bgets.

.. c:function:: int bsreadlna(bstring r, struct bStream * s, char terminator)
  
    Read a bstring terminated by the terminator character or the end of the
    stream from the bStream (s) and concatenate it to the parameter r.  The 
    matched terminator, if found, appears at the end of the line read.  If
    the stream has been exhausted of all available data, before any can be
    read, :c:macro:`BSTR_ERR` is returned.  This function may read additional characters 
    into the stream buffer from the core stream that are not returned, but 
    will be retained for subsequent read operations.  When reading from high 
    speed streams, this function can perform significantly faster than bgets.

.. c:function:: int bsreadlns(bstring r, struct bStream * s, bstring terminators)

    Read a bstring terminated by any character in the terminators bstring or 
    the end of the stream from the bStream (s) and return it into the 
    parameter r. This function may read additional characters from the core 
    stream that are not returned, but will be retained for subsequent read 
    operations.

.. c:function:: int bsreadlnsa(bstring r, struct bStream * s, bstring terminators)

    Read a bstring terminated by any character in the terminators bstring or 
    the end of the stream from the bStream (s) and concatenate it to the 
    parameter r.  If the stream has been exhausted of all available data, 
    before any can be read, :c:macro:`BSTR_ERR` is returned.  This function may read 
    additional characters from the core stream that are not returned, but 
    will be retained for subsequent read operations.

.. c:function:: int bsread(bstring r, struct bStream * s, int n)
  
    Read a bstring of length n (or, if it is fewer, as many bytes as is 
    remaining) from the bStream.  This function will read the minimum 
    required number of additional characters from the core stream.  When the
    stream is at the end of the file :c:macro:`BSTR_ERR` is returned, otherwise :c:macro:`BSTR_OK` 
    is returned.

.. c:function:: int bsreada(bstring r, struct bStream * s, int n)
  
    Read a bstring of length n (or, if it is fewer, as many bytes as is 
    remaining) from the bStream and concatenate it to the parameter r.  This 
    function will read the minimum required number of additional characters 
    from the core stream.  When the stream is at the end of the file :c:macro:`BSTR_ERR` 
    is returned, otherwise :c:macro:`BSTR_OK` is returned.
  
.. c:function:: int bsunread(struct bStream * s, const_bstring b)
  
    Insert a bstring into the bStream at the current position.  These 
    characters will be read prior to those that actually come from the core 
    stream.

.. c:function:: int bspeek(bstring r, const struct bStream * s)
  
    Return the number of currently buffered characters from the bStream that 
    will be read prior to reads from the core stream, and append it to the
    the parameter r.

.. c:function:: int bssplitscb(struct bStream * s, const_bstring splitStr, int (* cb) (void * parm, int ofs, const_bstring entry), void * parm)

    Iterate the set of disjoint sequential substrings over the stream s 
    divided by any character from the bstring splitStr.  The parm passed to 
    bssplitscb is passed on to cb.  If the function cb returns a value < 0, 
    then further iterating is halted and this return value is returned by 
    bssplitscb.

    Note: At the point of calling the cb function, the bStream pointer is 
    pointed exactly at the position right after having read the split 
    character.  The cb function can act on the stream by causing the bStream
    pointer to move, and bssplitscb will continue by starting the next split
    at the position of the pointer after the return from cb.

    However, if the cb causes the bStream s to be destroyed then the cb must
    return with a negative value, otherwise bssplitscb will continue in an 
    undefined manner.

    This function is provided as way to incrementally parse through a file
    or other generic stream that in total size may otherwise exceed the 
    practical or desired memory available.  As with the other split callback
    based functions this is abortable and does not impose additional memory 
    allocation.

.. c:function:: int bssplitstrcb(struct bStream * s, const_bstring splitStr, int (* cb) (void * parm, int ofs, const_bstring entry), void * parm)
	
    Iterate the set of disjoint sequential substrings over the stream s 
    divided by the entire substring splitStr.  The parm passed to 
    bssplitstrcb is passed on to cb.  If the function cb returns a 
    value < 0, then further iterating is halted and this return value is 
    returned by bssplitstrcb.

    Note: At the point of calling the cb function, the bStream pointer is 
    pointed exactly at the position right after having read the split 
    character.  The cb function can act on the stream by causing the bStream
    pointer to move, and bssplitstrcb will continue by starting the next 
    split at the position of the pointer after the return from cb.

    However, if the cb causes the bStream s to be destroyed then the cb must
    return with a negative value, otherwise bssplitscb will continue in an 
    undefined manner.

    This function is provided as way to incrementally parse through a file
    or other generic stream that in total size may otherwise exceed the 
    practical or desired memory available.  As with the other split callback
    based functions this is abortable and does not impose additional memory 
    allocation.
    
.. c:function:: int bseof(const struct bStream * s)

    Return the defacto "EOF" (end of file) state of a stream (1 if the 
    bStream is in an EOF state, 0 if not, and :c:macro:`BSTR_ERR` if stream is closed or 
    detectably erroneous.)  When the readPtr callback returns a value <= 0 
    the stream reaches its "EOF" state. Note that bunread with non-empty 
    content will essentially turn off this state, and the stream will not be 
    in its "EOF" state so long as its possible to read more data out of it.

    Also note that the semantics of bseof() are slightly different from 
    something like feof().  I.e., reaching the end of the stream does not
    necessarily guarantee that bseof() will return with a value indicating
    that this has happened.  bseof() will only return indicating that it has
    reached the "EOF" and an attempt has been made to read past the end of
    the bStream.

Macros
-------------

    The macros described below are shown in a prototype form indicating their
    intended usage.  Note that the parameters passed to these macros will be
    referenced multiple times.  As with all macros, programmer care is 
    required to guard against unintended side effects.

.. c:function:: int blengthe(const_bstring b, int err)

    Returns the length of the bstring.  If the bstring is :c:macro:`NULL` err is 
    returned.

.. c:function:: int blength(const_bstring b)

    Returns the length of the bstring.  If the bstring is :c:macro:`NULL`, the length 
    returned is 0.

.. c:function:: int bchare(const_bstring b, int p, int c)

    Returns the p'th character of the bstring b.  If the position p refers to 
    a position that does not exist in the bstring or the bstring is :c:macro:`NULL`, 
    then c is returned.

.. c:function:: char bchar(const_bstring b, int p)

    Returns the p'th character of the bstring b.  If the position p refers to 
    a position that does not exist in the bstring or the bstring is :c:macro:`NULL`, 
    then '\0' is returned.

.. c:function:: char * bdatae(bstring b, char * err)

    Returns the char * data portion of the bstring b.  If b is :c:macro:`NULL`, err is
    returned.

.. c:function:: char * bdata(bstring b)

    Returns the char * data portion of the bstring b.  If b is :c:macro:`NULL`, :c:macro:`NULL` is
    returned.

.. c:function:: char * bdataofse(bstring b, int ofs, char * err)

    Returns the char * data portion of the bstring b offset by ofs.  If b is 
    :c:macro:`NULL`, err is returned.

.. c:function:: char * bdataofs(bstring b, int ofs)

    Returns the char * data portion of the bstring b offset by ofs.  If b is 
    :c:macro:`NULL`, :c:macro:`NULL` is returned.

.. c:function:: void btfromblk(struct tagbstring& t, void * s, int len)

    Fill in the tagbstring t with the data buffer s with length len.  This 
    action is purely reference oriented; no memory management is done.  The 
    data member of t is just assigned s, and slen is assigned len.  Note that 
    the buffer is not appended with a '\0' character.  The s and len 
    parameters are accessed exactly once each in this macro.

    The resulting struct tagbstring is initially write protected.  Attempts 
    to write to this struct tagbstring in a write protected state from any 
    bstrlib function will lead to :c:macro:`BSTR_ERR` being returned.  Invoke the 
    bwriteallow on this struct tagbstring to make it writeable (though this 
    requires that s be obtained from a function compatible with malloc.)

.. c:function:: void btfromblkltrimws(struct tagbstring& t, void * s, int len)

    Fill in the tagbstring t with the data buffer s with length len after it
    has been left trimmed.  This action is purely reference oriented; no 
    memory management is done.  The data member of t is just assigned to a 
    pointer inside the buffer s.  Note that the buffer is not appended with a 
    '\0' character.  The s and len parameters are accessed exactly once each 
    in this macro.

    The resulting struct tagbstring is permanently write protected.  Attempts 
    to write to this struct tagbstring from any bstrlib function will lead to 
    :c:macro:`BSTR_ERR` being returned.  Invoking the bwriteallow macro onto this struct 
    tagbstring has no effect.

.. c:function:: void btfromblkrtrimws(struct tagbstring& t, void * s, int len)

    Fill in the tagbstring t with the data buffer s with length len after it
    has been right trimmed.  This action is purely reference oriented; no 
    memory management is done.  The data member of t is just assigned to a 
    pointer inside the buffer s.  Note that the buffer is not appended with a 
    '\0' character.  The s and len parameters are accessed exactly once each 
    in this macro.

    The resulting struct tagbstring is permanently write protected.  Attempts 
    to write to this struct tagbstring from any bstrlib function will lead to 
    :c:macro:`BSTR_ERR` being returned.  Invoking the bwriteallow macro onto this struct 
    tagbstring has no effect.

.. c:function:: void btfromblktrimws(struct tagbstring& t, void * s, int len)

    Fill in the tagbstring t with the data buffer s with length len after it
    has been left and right trimmed.  This action is purely reference 
    oriented; no memory management is done.  The data member of t is just 
    assigned to a pointer inside the buffer s.  Note that the buffer is not 
    appended with a '\0' character.  The s and len parameters are accessed 
    exactly once each in this macro.

    The resulting struct tagbstring is permanently write protected.  Attempts 
    to write to this struct tagbstring from any bstrlib function will lead to 
    :c:macro:`BSTR_ERR` being returned.  Invoking the bwriteallow macro onto this struct 
    tagbstring has no effect.

.. c:function:: void bmid2tbstr(struct tagbstring& t, bstring b, int pos, int len)

    Fill the tagbstring t with the substring from b, starting from position
    pos with a length len.  The segment is clamped by the boundaries of
    the bstring b.  This action is purely reference oriented; no memory 
    management is done.  Note that the buffer is not appended with a '\0' 
    character.  Note that the t parameter to this macro may be accessed 
    multiple times.  Note that the contents of t will become undefined 
    if the contents of b change or are destroyed.

    The resulting struct tagbstring is permanently write protected.  Attempts 
    to write to this struct tagbstring in a write protected state from any 
    bstrlib function will lead to :c:macro:`BSTR_ERR` being returned.  Invoking the
    bwriteallow macro on this struct tagbstring will have no effect.

.. c:function:: void bwriteprotect(struct tagbstring& t)

    Disallow bstring from being written to via the bstrlib API.  Attempts to 
    write to the resulting tagbstring from any bstrlib function will lead to 
    :c:macro:`BSTR_ERR` being returned.

    Note: bstrings which are write protected cannot be destroyed via bdestroy.

    Note to C++ users: Setting a CBString as write protected will not prevent
    it from being destroyed by the destructor.

.. c:function:: void bwriteallow(struct tagbstring& t)

    Allow bstring to be written to via the bstrlib API.  Note that such an 
    action makes the bstring both writable and destroyable.  If the bstring is
    not legitimately writable (as is the case for struct tagbstrings 
    initialized with a bsStatic value), the results of this are undefined.

    Note that invoking the bwriteallow macro may increase the number of 
    reallocs by one more than necessary for every call to bwriteallow 
    interleaved with any bstring API which writes to this bstring.

.. c:function:: int biswriteprotected(struct tagbstring& t)

    Returns 1 if the bstring is write protected, otherwise 0 is returned.

Internals
-----------------

.. warning::

    Modifying the internal values of bstrings can break safety guarentees.  Passing
    modified bstrings to code you haven't written is likely to cause random memory bugs
    to occur at random times.  *You have been warned.*

.. c:type:: struct tagbstring

    A bstring is basically a header which wraps a pointer to a char buffer.  Lets
    start with the declaration of a struct tagbstring:
    
    .. code-block:: c
        
        struct tagbstring {
            int mlen;
            int slen;
            unsigned char * data;
        };

    This definition is considered exposed, not opaque (though it is neither 
    necessary nor recommended that low level maintenance of bstrings be performed 
    whenever the abstract interfaces are sufficient).  The mlen field (usually)
    describes a lower bound for the memory allocated for the data field.  The 
    slen field describes the exact length for the bstring.  The data field is a 
    single contiguous buffer of unsigned chars.  Note that the existence of a '\0' 
    character in the unsigned char buffer pointed to by the data field does not 
    necessarily denote the end of the bstring.

    To be a well formed modifiable bstring the mlen field must be at least the 
    length of the slen field, and slen must be non-negative.  Furthermore, the 
    data field must point to a valid buffer in which access to the first mlen 
    characters has been acquired.  So the minimal check for correctness is:
    
    .. code-block:: c
        
        (slen >= 0 && mlen >= slen && data != NULL)

    bstrings returned by bstring functions can be assumed to be either NULL or
    satisfy the above property.  (When bstrings are only readable, the mlen >= 
    slen restriction is not required; this is discussed later in this section.)  
    A bstring itself is just a pointer to a struct tagbstring:
    
    .. code-block:: c
        
        typedef struct tagbstring * bstring;

    Note that use of the prefix "tag" in struct tagbstring is required to work
    around the inconsistency between C and C++'s struct namespace usage.  This
    definition is also considered exposed.

    Bstrlib basically manages bstrings allocated as a header and an associated 
    data-buffer.  Since the implementation is exposed, they can also be 
    constructed manually.  Functions which mutate bstrings assume that the header 
    and data buffer have been malloced; the bstring library may perform free() or 
    realloc() on both the header and data buffer of any bstring parameter. 
    Functions which return bstring's create new bstrings.  The string memory is 
    freed by a bdestroy() call (or using the bstrFree macro).

    The following related typedef is also provided:
    
    .. code-block:: c
        
        typedef const struct tagbstring * const_bstring;

    which is also considered exposed.  These are directly bstring compatible (no
    casting required) but are just used for parameters which are meant to be 
    non-mutable.  So in general, bstring parameters which are read as input but 
    not meant to be modified will be declared as const_bstring, and bstring
    parameters which may be modified will be declared as bstring.  This convention
    is recommended for user written functions as well.