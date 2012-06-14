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

	f = bfopen("empty.txt", "rb");
	TEST_EXPECT(f != NULL);
	TEST_EXPECT(bfgetc(f) == EOF);
	TEST_EXPECT(bfeof(f));
	bfclose(f);

	TEST_SUCCESS;
}