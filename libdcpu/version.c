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
#include "config.h"

bstring version_get()
{
#ifdef DCPU_CONFIG_HAS_VERSION_GIT
#ifdef DCPU_CONFIG_HAS_VERSION_STAGE
    return bfromcstr(DCPU_CONFIG_VERSION_STAGE "; version #" DCPU_CONFIG_VERSION_GIT);
#else
    return bfromcstr("version #" DCPU_CONFIG_VERSION_GIT);
#endif
#else
#ifdef DCPU_CONFIG_VERSION_STAGE
    return bfromcstr(DCPU_CONFIG_VERSION_STAGE);
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
