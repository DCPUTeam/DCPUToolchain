/**
 *
 *	File:		string.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Standard Library
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Provides string manipulation functions.
 *
 **/

#include "stdlib.h"
#include "string.h"

void* memcpy(void* destination, const void* source, size_t num)
{
	size_t i;
	for (i = 0; i < num; i++)
		destination[i] = source[i];
	return destination;
}

void* memmove(void* destination, const void* source, size_t num)
{
	size_t i;
	void* temp = malloc(num);
	for (i = 0; i < num; i++)
		temp[i] = source[i];
	for (i = 0; i < num; i++)
		destination[i] = temp[i];
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
		if (first[i] > second[i])
			return 1;
		else if (first[i] < second[i])
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
    /*
    unsigned char* str=(unsigned char*)ptr;
    unsigned char c = (unsigned char) value;
    while(num--) {
        if (*str==c)
            return (void*)str;
    }
    */
    return NULL;
}

char* strchr(const char* ptr, int value)
{
    /*
    // this function by definition is insecure, use with caution!!
    unsigned char* str=(unsigned char*)ptr;
    unsigned char c = (unsigned char) value;
    while(1) {
        if (*str==c)
            return (void*)str;
    }
    */
    return NULL;
}

size_t strcspn(const char* s, const char* accept)
{
    /* submitted broken code
    // TODO: check this for basic functionality
    // I am not sure if this even fits what the standard calls for
    // though it seems to
    size_t len = 0;
    size_t alen = strlen(accept);
    unsigned char* str=(unsigned char*)s;
    // loop over string
    while(*(str++)!=0) {
        const char cur = *str;
        size_t i;
        size_t count = 0;
        for (i=0;i<alen;i++) {
            if(cur == accept[i]) {
                count++;
                break;
            }
        }
        if (!count)
            return len;
        len++;
    }
    */
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
		destination[i] = value;
	return destination;
}

size_t strlen(const char* str)
{
	size_t i = 0;
	while (str[i++] != 0) ;
	return i;
}
