/**

    File:       version.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Defines the functions that provide version
            information.

**/

#include <stdio.h>
#include "debug.h"
#include "bstring.h"
#include "version.h"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)
#define STRING_VERSION_GIT EXPAND_AND_QUOTE(VERSION_GIT)
#define STRING_VERSION_STAGE EXPAND_AND_QUOTE(VERSION_STAGE)

bstring version_get()
{
#ifdef VERSION_GIT
    return bfromcstr(STRING_VERSION_STAGE "; version #" STRING_VERSION_GIT);
#else
#ifdef VERSION_STAGE
    return bfromcstr(STRING_VERSION_STAGE);
#else
    return bfromcstr("unknown");
#endif
#endif
}

void version_print(freed_bstring application)
{
    bstring ver = version_get();

    if (debug_getlevel() < LEVEL_VERBOSE)
    {
        bdestroy(ver);
        bautodestroy(application);
        return;
    }

    fprintf(stderr, "DCPU-16 Toolchain ");
    fprintf(stderr, "%s", application.ref->data);
    fprintf(stderr, " (");
    fprintf(stderr, "%s", ver->data);
    fprintf(stderr, ")\n");
    fprintf(stderr, "This software is MIT licensed.\n");
    fprintf(stderr, "\n");

    bdestroy(ver);
    bautodestroy(application);
}
