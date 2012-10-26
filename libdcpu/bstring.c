/**

    File:       bstring.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Defines utility methods for interacting
            with freed_bstring type.

**/

#include "bstring.h"

freed_bstring bautofree(bstring str)
{
    freed_bstring result;
    result.ref = str;
    result.dummy = 0;
    return result;
}

void bautodestroy(freed_bstring f)
{
    bdestroy(f.ref);
}

freed_bstring bautocpy(const_bstring b1)
{
    return bautofree(bstrcpy(b1));
}

int bescape(bstring str)
{
    // TODO: Handle all other non-printable characters by using \0xXXXX format.
    if (bfindreplace(str, bfromcstr("\\"), bfromcstr("\\\\"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\0"), bfromcstr("\\0"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\a"), bfromcstr("\\a"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\b"), bfromcstr("\\b"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\f"), bfromcstr("\\f"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\n"), bfromcstr("\\n"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\r"), bfromcstr("\\r"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\t"), bfromcstr("\\t"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\v"), bfromcstr("\\v"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\'"), bfromcstr("\\'"), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\""), bfromcstr("\\\""), 0) == BSTR_ERR) return BSTR_ERR;
    if (bfindreplace(str, bfromcstr("\?"), bfromcstr("\?"), 0) == BSTR_ERR) return BSTR_ERR;
    return BSTR_OK;
}
