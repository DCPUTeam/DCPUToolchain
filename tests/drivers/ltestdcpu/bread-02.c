/**

	File:		bread-02.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Tests reading the first character in an
			empty file.

**/

#include <bfile.h>
#include "tests.h"

int bread_02()
{
	BFILE* f;

	f = bopen("empty.txt", "rb");
	TEST_EXPECT(f != NULL);
	TEST_EXPECT(bgetc(f) == EOF);
	TEST_EXPECT(beof(f));
	bclose(f);

	TEST_SUCCESS;
}