/**

    File:       btell-02.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU Test Suite

    Authors:    James Rhodes

    Description:    Tests retrieval of the current position on
            an empty file.

**/

#include <bfile.h>
#include "tests.h"

int btell_02()
{
    BFILE* f;

    f = bfopen("empty.txt", "rb");
    TEST_EXPECT(f != NULL);
    TEST_EXPECT(bftell(f) == 0);
    bfclose(f);

    TEST_SUCCESS;
}