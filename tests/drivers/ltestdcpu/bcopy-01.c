/**

    File:       bcopy-01.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU Test Suite

    Authors:    James Rhodes

    Description:    Tests the buffering mechanism to test whether
            or not files can be copied character-by-character
            in a while loop.

**/

#include <bfile.h>
#include "tests.h"

int bcopy_01()
{
    BFILE* in;
    BFILE* out;
    int i = 0;

    in = bfopen("test.txt", "rb");
    TEST_EXPECT(in != NULL);
    out = bfopen("copy.txt", "wb");
    TEST_EXPECT(out != NULL);

    while (!bfeof(in))
    {
        bfputc(bfgetc(in), out);
        i++;
    }

    bfclose(in);
    bfclose(out);

    if (i == LENGTH_OF_TEST_FILE)
    {
        TEST_SUCCESS;
    }
    else
    {
        TEST_FAILURE("didn't copy all expected data (total bytes copied incorrect).");
    }
}