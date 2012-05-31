/**

	File:		tests.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU Test Suite

	Authors:	James Rhodes

	Description:	Defines available tests.

**/

#ifndef __DCPU_TESTS_H
#define __DCPU_TESTS_H

#define TEST_EXPECT(expr) if (!(expr)) { printd(LEVEL_ERROR, "test expect failed: %s\n.", #expr); return 1; }
#define TEST_SUCCESS { return 0; }
#define TEST_FAILURE(message) { printd(LEVEL_ERROR, "test failed: %s\n.", message); return 1; }
#define LENGTH_OF_TEST_FILE 72
#define FIRST_CHAR_OF_TEST_FILE 'T'

#include <debug.h>

int bopen_01();
int bopen_02();
int bread_01();
int bread_02();
int bcopy_01();
int btell_01();
int btell_02();

#endif