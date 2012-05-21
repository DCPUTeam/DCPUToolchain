/**
 * 
 *	File:		string.h
 * 
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Standard Library
 * 
 *	Authors:	James Rhodes
 * 
 *	Description:	Provides string manipulation functions.
 * 
 **/

#ifndef __STDLIB_STRING_H
#define __STDLIB_STRING_H

#ifndef size_t
#define size_t uint16_t
#endif
#ifndef NULL
#define NULL 0
#endif

// Copying:
void* memcpy(void* destination, const void* source, size_t num);
void* memmove(void* destination, const void* source, size_t num);
char* strcpy(char* destination, const char* source);
char* strncpy(char* destination, const char* source, size_t num);

// Concatentation:
char* strcat(char* destination, const char* source);
char* strncat(char* destination, char* source, size_t num);

// Comparison:
int memcmp(const void* first, const void* second, size_t num);
int strcmp(const char* first, const char* second);
int strncmp(const char* first, const char* second, size_t num);

// Searching:
void* memchr(const void* ptr, int value, size_t num);
char* strchr(const char* ptr, int value);
size_t strcspn(const char* first, const char* second);
char* strpbrk(const char* first, const char* second);
char* strrchr(const char* ptr, int value);
size_t strspn(const char* first, const char* second);
char* strstr(const char* str, const char* find);
char* strtok(char* str, const char* delimiters);

// Other:
void* memset(void* ptr, int value, size_t num);
char* strerror(int errnum);
size_t strlen(const char* str);

#endif
