/**

	File:           pp.h

	Project:        DCPU-16 Tools
	Component:      Compiler

	Authors:        James Rhodes

	Description:    Declares the functions used by the
	                compiler for preprocessing.

**/

#ifndef __DCPU_COMP_PREPROCESSOR_H
#define __DCPU_COMP_PREPROCESSOR_H

void pp_add_search_path(const char* path);
FILE* pp_do(const char* input);
void pp_cleanup();

#endif
