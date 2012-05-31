/**

	File:		bopen-01.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Tests the bopen() and bclose() methods
			to ensure that they correctly open and close
			files.

**/

#include <bfile.h>
#include "tests.h"

int bopen_01()
{
	BFILE* f;

	f = bopen("nonexistant.blah", "rb");
	TEST_EXPECT(f == NULL);
	f = bopen("test.txt", "rb");
	TEST_EXPECT(f != NULL);
	bclose(f);

	TEST_SUCCESS;
}