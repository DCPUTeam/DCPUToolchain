/**
 *
 *	File:		stdlib.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Standard Library
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Provides standard library functions.
 *
 **/

#include "stdlib.h"

// String conversion:
int atoi(const char* str)
{
	// TODO
	return 0;
}

// Pseudo-random sequence generation:
int rand()
{
	return 4;	// Chosen by fair dice roll.
	// Guaranteed to be random.
}

void srand(unsigned int seed)
{
	// TODO
}

// Dynamic memory management:
void* calloc(size_t num, size_t size)
{
	return malloc(num * size);
}

void free(void* ptr)
{
	__asm
	{
		;
		The compiler needs to support variable insertion.
		;
		SET A, <ptr>
		;
		JSR free
	}
}

void* malloc(size_t size)
{
	void* result;
	__asm
	{
		;
		The compiler needs to support variable insertion.
		;
		SET A, <size>
		;
		JSR malloc
		;
		SET <&result>, A
	}
	return result;
}

void* realloc(void* ptr, size_t size)
{
	void* result;
	__asm
	{
		;
		The compiler needs to support variable insertion.
		;
		SET A, <ptr>
		;
		SET B, <size>
		;
		JSR malloc
		;
		SET <&result>, A
	}
	return result;
}

// Environment:
void abort()
{
	__asm
	{
		JSR _halt
	}
}

// Searching and sorting:
void* bsearch(const void* key, const void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b))
{
	// TODO
	return NULL;
}

void qsort(void* base, size_t num, size_t size, int (*comparator)(const void* a, const void* b))
{
	// TODO
}

// Integer arithmethics:
int abs(int n)
{
	if (n < 0)
		return -n;
	else
		return n;
}

struct div_s div(int numerator, int denominator)
{
	struct div_s result;
	result.quot = numerator / denominator;
	result.rem = numerator % denominator;
	return result;
}

