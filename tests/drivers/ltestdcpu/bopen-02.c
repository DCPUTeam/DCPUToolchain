/**

	File:		bopen-02.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Tests the bopen() and bclose() methods
			to ensure that they correctly open and close
			files.

**/

#ifndef WIN32
#include <unistd.h>
#endif
#include <bfile.h>
#include "tests.h"

int bopen_02()
{
	BFILE* f;

	f = bfopen("nonexistant.blah", "wb");
	TEST_EXPECT(f != NULL);
	bfclose(f);
	unlink("nonexistant.blah");

	TEST_SUCCESS;
}
