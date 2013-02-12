/**
 *
 *  File:           stdarg.h
 *
 *  Project:        DCPU-16 Toolchain
 *  Component:      C Standard Library
 *
 *  Authors:        Patrick Flick
 *
 *  Description:    The macros defined in this header allow functions
 *                  to accept an indefinite number of arguments
 *
 **/
#ifndef __STDLIB_STDARG_H
#define __STDLIB_STDARG_H

// some pp bug, i need to do this twice, otherwise the va_list doesn't get replaced in other files
#define va_list void*

// use built-in va_start, needs access to function parameter stack
#define va_start(list,nparam)     list = ((void*)&nparam) + sizeof(nparam)

// cast the void* to pointer to wanted type and deref, then increase
// va list pointer
#define va_arg(list,type)       *((type *) list); list+=sizeof(type)

// va_end doesn't do anything at all
#define va_end(list)


#endif
