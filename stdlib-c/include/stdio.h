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

/**
 * @brief Write formatted data to string
 * @param str Pointer to a buffer where the resulting C-string is stored. The buffer should be large enough to contain the resulting string.
 * @param format C string that contains a format string that follows the same specifications as format in printf
 * @param ... Depending on the format string, the function may expect a sequence of additional arguments, each containing a value to be used to replace a format specifier in the format string (or a pointer to a storage location, for n). There should be at least as many of these arguments as the number of values specified in the format specifiers. Additional arguments are ignored by the function.
 * @returns On success, the total number of characters written is returned. This count does not include the additional null-character automatically appended at the end of the string.
            On failure, a negative number is returned.
 * 
 * Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, the content is stored as a C string in the buffer pointed by s. The size of the buffer should be large enough to contain the entire resulting string. A terminating null character is automatically appended after the content. After the format parameter, the function expects at least as many additional arguments as needed for format.
 */
int sprintf(char * str, const char * format, ...);

#endif
