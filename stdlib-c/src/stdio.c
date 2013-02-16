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

#include "stdio.h"
#include <stdarg.h>

void uitodec(unsigned int value, char * str)
{
    int length;
    unsigned int tmp;

    if (value == 0)
    {
        *(str++) = '0';
        *str = '\0';
        return;
    }

    tmp = value;
    length = 0;
    while(tmp > 0)
    {
        length++;
        tmp /= 10;
    }

    str += length-1;

    while(value > 0)
    {
        *(str--) = (value % 10) + '0';
        value /= 10;
    }
    
    str += length+1;
    *(str) = '\0';
}

void itodec(int value, char * str)
{
    if (value < 0)
    {
        *(str++) = '-';
        value = -value;
    }
    
    uitodec(value, str);
}

void itohex(unsigned int value, char * str, int uppercase)
{
    int length;
    unsigned int tmp;
    int digit;

    if (value == 0)
    {
        *(str++) = '0';
        *str = '\0';
        return;
    }

    tmp = value;
    length = 0;
    while(tmp > 0)
    {
        length++;
        tmp /= 0x10;
    }

    str += length-1;

    while(value > 0)
    {
        digit = (value % 0x10);
        if (digit >= 10)
            if (uppercase)
                *(str--) = (digit-10) + 'A';
            else
                *(str--) = (digit-10) + 'a';
        else
            *(str--) = digit + '0';
        value /= 0x10;
    }
    
    str += length+1;
    *(str) = '\0';
}

void itooct(unsigned int value, char * str)
{
    int length;
    unsigned int tmp;

    if (value == 0)
    {
        *(str++) = '0';
        *str = '\0';
        return;
    }

    tmp = value;
    length = 0;
    while(tmp > 0)
    {
        length++;
        tmp /= 0x8;
    }

    str += length-1;

    while(value > 0)
    {
        *(str--) = (value % 0x8) + '0';
        value /= 0x8;
    }
    
    str += length+1;
    *(str) = '\0';
}


char * itoa(int value, char * str, int base)
{
    if (base < 2 || base > 36)
        // not handled, there is no error code
        return str;
        
    if (base == 10)
        itodec(value, str);
    else if (base == 8)
        itooct(value, str);
    else if (base == 0x10)
        itohex(value, str, 0);
    else
    {
        // TODO
    }
    return str;
}


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
int sprintf ( char * str, const char * format, ... )
{
    /*
     * %[flags][width][.precision][length]specifier
     * 
     * Right now no optional arguments are supported yet (i.e. specifier only)
     */
    char strbufmem[17];
    int int_val;
    unsigned int uint_val;
    char char_val;
    char * str_val;
    int * intp_val;
    va_list vl;
    char * str_start = str;
    char* strbuf = strbufmem;
    
    va_start(vl, format);
    
    while (*format != '\0')
    {
        if (*format == '%')
        {
            format++;
            
            switch(*format)
            {
                case 'd':
                case 'i':
                    int_val = va_arg(vl,int);
                    itodec(int_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'u':
                    uint_val = va_arg(vl,unsigned int);
                    uitodec(uint_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'o':
                    uint_val = va_arg(vl,unsigned int);
                    itooct(uint_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'x':
                    uint_val = va_arg(vl,unsigned int);
                    itohex(uint_val, str, 0);
                    while (*str != '\0') { str++;}
                    break;
                case 'X':
                case 'p': // treat the pointer '%p' as an uint (which is legal on the DCPU)
                    uint_val = va_arg(vl,unsigned int);
                    itohex(uint_val, str, 1);
                    while (*str != '\0') { str++;}
                    break;
                case 'f':
                case 'F':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                case 'a':
                case 'A':
                    // TODO when floats are implemented
                    // for now just ignore
                    uint_val = va_arg(vl,unsigned int);
                    break;
                case 'c':
                    char_val = va_arg(vl,char);
                    *(str++) = char_val;
                    break;
                case 's':
                    str_val = va_arg(vl,char*);
                    while(*str_val != '\0')
                        *(str++) = *(str_val++);
                    break;
                case 'n':
                    intp_val = va_arg(vl,int*);
                    // TODO needs casting :)
                    //*intp_val = str - str_start;
                    break;
                case '%':
                    *(str++) = '%';
                    break;
                default:
                    ;
                    // dont do anything
            }
            format++;
        }
        else
        {
            *(str++) = *(format++);
        }
    }
    
    *str = '\0';
    
    va_end(vl);
}
