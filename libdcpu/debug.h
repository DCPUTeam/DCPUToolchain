/**

    File:       debug.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Declares functions for logging and determining
            the level of logging appropriate.

**/

#ifndef __DCPU_DEBUG_H
#define __DCPU_DEBUG_H

#include <stdarg.h>

#define LEVEL_EVERYTHING 7
#define LEVEL_DEBUG 6
#define LEVEL_VERBOSE 5
#define LEVEL_DEFAULT 4
#define LEVEL_WARNING 3
#define LEVEL_ERROR 2
#define LEVEL_CRITICAL 1
#define LEVEL_SILENT 0

void printd(int level, const char* message, ...);
void vprintd(int level, const char* message, va_list args);
void debug_setlevel(int level);
int debug_getlevel();

#endif
