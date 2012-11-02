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

void uitodec(unsigned int value, char * str)
{
    int length;
    unsigned int tmp;

    if (value == 0)
    {
        *(str++) = '0';
        *str = '\0';
        return 0;
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
        return 0;
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
        return 0;
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

// FIXME use macros from stdarg
#define va_list void*

/*
 * %[flags][width][.precision][length]specifier
 * 
 * Right now no optional arguments are supported yet (i.e. specifier only)
 */
int sprintf ( char * str, const char * format, ... )
{
    char strbufmem[17];
    int int_val;
    unsigned int uint_val;
    char char_val;
    char * str_val;
    int * intp_val;
    va_list vl;
    char * str_start = str;
    char* strbuf = strbufmem;
    
    
    // FIXME: use macros
    // va_start(vl, format);
    __builtin_va_start(vl,format);
    
    while (*format != '\0')
    {
        if (*format == '%')
        {
            format++;
            
            switch(*format)
            {
                case 'd':
                case 'i':
                    // FIXME: use macros
                    // va_arg(vl,int);
                    int_val = *vl; vl+=sizeof(int);
                    itodec(int_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'u':
                    // FIXME: use macros
                    // va_arg(vl,unsigned int);
                    uint_val = *vl; vl+=sizeof(unsigned int);
                    uitodec(uint_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'o':
                    // FIXME: use macros
                    // va_arg(vl,unsigned int);
                    uint_val = *vl; vl+=sizeof(unsigned int);
                    itooct(uint_val, str);
                    while (*str != '\0') { str++;}
                    break;
                case 'x':
                    // FIXME: use macros
                    // va_arg(vl,unsigned int);
                    uint_val = *vl; vl+=sizeof(unsigned int);
                    itohex(uint_val, str, 0);
                    while (*str != '\0') { str++;}
                    break;
                case 'X':
                case 'p': // treat the pointer '%p' as an uint (which is legal on the DCPU)
                    // FIXME: use macros
                    // va_arg(vl,unsigned int);
                    uint_val = *vl; vl+=sizeof(unsigned int);
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
                    uint_val = *vl; vl+=sizeof(unsigned int);
                    break;
                case 'c':
                    // FIXME: use macros
                    // va_arg(vl,char);
                    char_val = *vl; vl+=sizeof(char);
                    *(str++) = char_val;
                    break;
                case 's':
                    // FIXME: use macros
                    // va_arg(vl,char*);
                    str_val = *vl; vl+=sizeof(char*);
                    while(*str_val != '\0')
                        *(str++) = *(str_val++);
                    break;
                case 'n':
                    // FIXME: use macros
                    // va_arg(vl,int*);
                    intp_val = *vl; vl+=sizeof(int*);
                    // TODO needs casting :)
                    //*intp_val = str - str_start;
                    break;
                case '%':
                    *(str++) = '%';
                    break;
                default:
                    // dont do anything
            }
            format++;
        }
        else
        {
            *(str++) = *(format++);
        }
    }
    
    // FIXME use macros
    // va_end(vl);
}
