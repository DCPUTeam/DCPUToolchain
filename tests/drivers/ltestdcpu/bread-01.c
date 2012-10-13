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

	f = bfopen("test.txt", "rb");
	TEST_EXPECT(f != NULL);
	TEST_EXPECT(bfgetc(f) == FIRST_CHAR_OF_TEST_FILE);
	bfclose(f);

	TEST_SUCCESS;
}