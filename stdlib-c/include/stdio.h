/**
 *
 *  File:           stdio.h
 *
 *  Project:        DCPU-16 Toolchain
 *  Component:      C Standard Library
 *
 *  Authors:        Patrick Flick
 *
 *  Description:    Provides input and output functions.
 *
 **/

#ifndef __STDLIB_STDIO_H
#define __STDLIB_STDIO_H

char * itoa(int value, char * str, int base);
int sprintf ( char * str, const char * format, ... );

#endif
