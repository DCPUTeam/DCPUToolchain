/**

	File:		bread-01.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Tests reading the single first character
			from a file.

**/

#include <bfile.h>
#include "tests.h"

int bread_01()
{
	BFILE* f;

	f = bopen("test.txt", "rb");
	TEST_EXPECT(f != NULL);
	TEST_EXPECT(bgetc(f) == FIRST_CHAR_OF_TEST_FILE);
	bclose(f);

	TEST_SUCCESS;
}