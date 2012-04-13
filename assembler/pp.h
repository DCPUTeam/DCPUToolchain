/**

	File:			pp.h

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Declares the functions used by the
					assembler for preprocessing.

**/

#ifndef __DCPU_ASM_PREPROCESSOR_H
#define __DCPU_ASM_PREPROCESSOR_H

#define PATH_COUNT_MAX 100
extern int pp_path_count;
extern const char* pp_path_names[PATH_COUNT_MAX];

void pp_add_search_path(const char* path);
FILE* pp_do(const char* input);
void pp_cleanup();

#endif