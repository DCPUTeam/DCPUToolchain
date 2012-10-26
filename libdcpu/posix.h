/**

    File:       posix.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Provides macros that allow DCPU-Toolchain to work
            on POSIX platforms.

**/

// POSIX declares their case-insensitive string comparison function
// as strcasecmp instead of stricmp (which is not available there).
// If we're on POSIX, provide a macro to alias this function.
#ifndef WIN32
#define stricmp strcasecmp
#endif

