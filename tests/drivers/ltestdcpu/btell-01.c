/**

	File:		btell-01.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Tests retrieval of the current position in
			the file.

**/

#include <bfile.h>
#include "tests.h"

int btell_01()
{
	BFILE* f;
	int i = 0;

	f = bopen("test.txt", "rb");
	TEST_EXPECT(f != NULL);
	TEST_EXPECT(btell(f) == 0);

	while (!beof(f))
	{
		bgetc(f);
		i++;
		TEST_EXPECT(btell(f) == i);
	}

	bclose(f);

	TEST_SUCCESS;
}