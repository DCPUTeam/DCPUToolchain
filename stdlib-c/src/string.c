/**
 *
 *      File:           string.c
 *
 *      Project:        DCPU-16 Toolchain
 *      Component:      C Standard Library
 *
 *      Authors:        James Rhodes
 *
 *      Description:    Provides string manipulation functions.
 *
 **/

#include "stdlib.h"
#include "string.h"

void* memcpy(void* destination, const void* source, size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
        ((char*)destination)[i] = ((char*)source)[i];
    return destination;
}

void* memmove(void* destination, const void* source, size_t num)
{
    size_t i;
    void* temp = malloc(num);
    for (i = 0; i < num; i++)
        ((char*)temp)[i] = ((char*)source)[i];
    for (i = 0; i < num; i++)
        ((char*)destination)[i] = ((char*)temp)[i];
    free(temp);
    return destination;
}

char* strcpy(char* destination, const char* source)
{
    while (*source != 0)
        *(destination++) = *(source++);
    return destination;
}

char* strncpy(char* destination, const char* source, size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
    {
        if (*source == 0)
            *(destination++) = 0;
        else
            *(destination++) = *(source++);
    }
    return destination;
}

// Concatentation:
char* strcat(char* destination, const char* source)
{
    size_t len = strlen(destination);
    destination += len;
    while (*source != 0)
        *(destination++) = *(source++);
    return destination;
}

char* strncat(char* destination, char* source, size_t num)
{
    size_t len = strlen(destination);
    size_t i = 0;
    destination += len;
    while (*source != 0 && i++ < num)
        *(destination++) = *(source++);
    return destination;
}

// Comparison:
int memcmp(const void* first, const void* second, size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
    {
        if (((char*)first)[i] > ((char*)second)[i])
            return 1;
        else if (((char*)first)[i] < ((char*)second)[i])
            return -1;
    }
    return 0;
}

int strcmp(const char* first, const char* second)
{
    size_t i;
    while (first[i] != 0 && second[i] != 0)
    {
        if (first[i] > second[i])
            return 1;
        else if (first[i] < second[i])
            return -1;
        i++;
    }
    return 0;
}

int strncmp(const char* first, const char* second, size_t num)
{
    size_t i;
    while (first[i] != 0 && second[i] != 0 && i++ < num)
    {
        if (first[i] > second[i])
            return 1;
        else if (first[i] < second[i])
            return -1;
    }
    return 0;
}

// Searching:
void* memchr(const void* ptr, int value, size_t num)
{
    // TODO
    return NULL;
}

char* strchr(const char* ptr, int value)
{
    // TODO
    return NULL;
}

size_t strcspn(const char* first, const char* second)
{
    // TODO
    return 0;
}

char* strpbrk(const char* first, const char* second)
{
    // TODO
    return NULL;
}

char* strrchr(const char* ptr, int value)
{
    // TODO
    return NULL;
}

size_t strspn(const char* first, const char* second)
{
    // TODO
    return 0;
}

char* strstr(const char* str, const char* find)
{
    // TODO
    return NULL;
}

char* strtok(char* str, const char* delimiters)
{
    // TODO
    return NULL;
}

// Other:
void* memset(void* destination, int value, size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
        ((int*)destination)[i] = value;
    return destination;
}

size_t strlen(const char* str)
{
    size_t i = 0;
    while (str[i++] != 0) ;
    return i;
}
