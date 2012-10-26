/**

    File:       iio.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Implements reading and writing assembler
            instructions in the correct endianness.

**/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bfile.h"
#include "iio.h"

int icpumode = IMODE_UNKNOWN;
int iwritemode = IMODE_BIG;

void icpudetect()
{
    uint16_t val = 0x7C01;
    char mem[2] = { 0, 0 };
    if (icpumode != IMODE_UNKNOWN)
        return;
    memcpy(mem, &val, sizeof(uint16_t));
    if (mem[0] == 0x7C)
        icpumode = IMODE_BIG;
    else
        icpumode = IMODE_LITTLE;
}

void isetmode(int mode)
{
    assert(mode != IMODE_UNKNOWN);
    iwritemode = mode;
}

uint16_t iflip(uint16_t s)
{
    unsigned char c1, c2;
    c1 = s & 255;
    c2 = (s >> 8) & 255;
    return (c1 << 8) + c2;
}

size_t iwrite(const uint16_t* src, FILE* out)
{
    uint16_t inst;

    // Test endianness if we haven't already.
    if (icpumode == IMODE_UNKNOWN)
        icpudetect();
    assert(icpumode != IMODE_UNKNOWN);

    // See if we need to flip bytes.
    if (icpumode != iwritemode)
        inst = iflip(*src);
    else
        inst = *src;

    // Write word.
    return fwrite(&inst, sizeof(uint16_t), 1, out);
}

size_t iread(uint16_t* dest, FILE* in)
{
    size_t result;

    // Test endianness if we haven't already.
    if (icpumode == IMODE_UNKNOWN)
        icpudetect();
    assert(icpumode != IMODE_UNKNOWN);

    // Read word.
    result = fread(dest, sizeof(uint16_t), 1, in);

    // See if we need to flip bytes.
    if (icpumode != iwritemode)
        *dest = iflip(*dest);

    // Return result.
    return result;
}