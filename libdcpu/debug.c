/**

    File:       debug.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Declares functions for logging and determining
            the level of logging appropriate.

**/

#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

int _current = LEVEL_DEFAULT;

///
/// Potentially outputs a debugging message to stderr.
///
/// Potentially outputs a debugging message to stderr depending
/// on the current debugging level.
///
/// @param level The debugging level relevant for this output.
/// @param message The message format (same as printf).
///
void printd(int level, const char* message, ...)
{
    va_list argptr;
    va_start(argptr, message);
    if (_current >= level && _current != LEVEL_SILENT)
        vfprintf(stderr, message, argptr);
    va_end(argptr);
}

///
/// Potentially outputs a debugging message to stderr.
///
/// Potentially outputs a debugging message to stderr depending
/// on the current debugging level.
///
/// @param level The debugging level relevant for this output.
/// @param message The message format (same as printf).
/// @param args The variable argument list.
///
void vprintd(int level, const char* message, va_list args)
{
    if (_current >= level && _current != LEVEL_SILENT)
        vfprintf(stderr, message, args);
}

///
/// Sets the debugging level.
///
/// @param level The new debugging level.
///
void debug_setlevel(int level)
{
    _current = level;
}

///
/// Gets the current debugging level.
///
int debug_getlevel()
{
    return _current;
}
