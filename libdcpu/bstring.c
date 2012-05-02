/**

	File:		bstring.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Defines utility methods for interacting
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
