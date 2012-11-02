/**

    File:           ppcompat.h

    Project:        DCPU-16 Toolchain
    Component:      LibDCPU-PP

    Authors:        James Rhodes

    Description:    Declares the compatibility API for
                    programs using the old preprocessor API.

**/

#ifndef __DCPU_LIBDCPU_PP_COMPAT_H
#define __DCPU_LIBDCPU_PP_COMPAT_H

#include <bstring.h>

bstring pp_do(freed_bstring lang, freed_bstring path);
void pp_cleanup(freed_bstring path);

#endif
