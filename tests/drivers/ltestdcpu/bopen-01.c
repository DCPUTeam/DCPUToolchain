/**

    File:       bopen-01.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU Test Suite

    Authors:    James Rhodes

    Description:    Tests the bopen() and bclose() methods
            to ensure that they correctly open and close
            files.

**/

#include <bfile.h>
#include "tests.h"

int bopen_01()
{
    BFILE* f;

    f = bfopen("nonexistant.blah", "rb");
    TEST_EXPECT(f == NULL);
    f = bfopen("test.txt", "rb");
    TEST_EXPECT(f != NULL);
    bfclose(f);

    TEST_SUCCESS;
}