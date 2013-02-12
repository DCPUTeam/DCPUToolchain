/**
 *
 *	File:		screen.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Extended Library
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Provides access to the screen.
 *
 **/

#include "ext/screen.h"

void eputc(char chr, int x, int y)
{
    eputxc(chr, x, y, 0xF000);
}

void eputs(const char* string, int x, int y)
{
    eputxs(string, x, y, 0xF000);
}

void eputxc(char chr, int x, int y, int modifier)
{
    char* mem = (char*) (0x8000 + x + y * 32);
    *mem = chr + modifier;
}

void eputxs(const char* string, int x, int y, int modifier)
{
    int i = 0;
    for (i = 0; * (string + i) != '\0'; i += 1)
        eputxc(*(string + i), x + i, y, modifier);
}
