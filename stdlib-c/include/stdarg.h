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

#define va_list unsigned int


// FIXME we need preprocessor macros for this to work :D

//#define va_start(list,nparam)   __builtin_va_start(list)

// FIXME uncomment as soon as explicit casting is implemented
//#define va_arg(list,type)       *((type)*) &(list); list+=sizeof(type)
//#define va_arg(list,type)       list; list+=sizeof(type)
// va_end doesn't do anything at all
//#define va_end(list)            


#endif
