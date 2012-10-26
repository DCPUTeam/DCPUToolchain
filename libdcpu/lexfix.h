/**

    File:       lexfix.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Defines fixes for the lexer / parser system
            that are required to run under a system where
            warnings cause errors.

**/

#ifdef _WIN32
#pragma warning(disable:4005;disable:4013;disable:4018)
#endif
