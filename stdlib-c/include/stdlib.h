/**
 * 
 *	File:		stdlib.h
 * 
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Standard Library
 * 
 *	Authors:	James Rhodes
 * 
 *	Description:	Provides standard library functions.
 * 
 **/

#ifndef __STDLIB_H
#define __STDLIB_H

#define RAND_MAX 32767
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MB_CUR_MAX 1
#ifndef NULL
#define NULL 0
#endif
#ifndef size_t
#define size_t unsigned short
#endif

// Structures:
struct div_s
{
	int quot;
	int rem;
};

// String conversion:
//double atof(const char* str);
int atoi(const char* str);
//long int atol(const char* str);
//double strtod(const char* str, char** endptr);
//long int strtol(const char* str, char** endptr, int base);
//unsigned long int strtoul(const char* str, char** endptr, int base);

// Pseudo-random sequence generation:
int rand();
void srand(unsigned int seed);

// Dynamic memory management:
void* calloc(size_t num, size_t size);
void free(void* ptr);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);

// Environment:
void abort();
//int atexit(void (*function)(void) );
//void exit(int status);
//char* getenv(const char* name);
//int system(const char* command);

// Searching and sorting:
void* bsearch(const void* key, const void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b));
void qsort(void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b));

// Integer arithmethics:
int abs(int n);
struct div_s div(int numerator, int denominator);
//long int labs(long int n);
//ldiv_t ldiv(long int numerator, long int denominator);

//typedef struct
//{
//	long int quot;
//	long int rem;
//} ldiv_t;

#endif
