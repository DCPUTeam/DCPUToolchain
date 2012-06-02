/**

	File:		class.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Provides macros and functions for object-based
			infrastructures.

**/

#ifndef __DCPU_CLASS_H
#define __DCPU_CLASS_H

#define CLASS_TYPE(type)	const char* typename;
#define CLASS_OPERATORS(type)
#define CLASS_INIT(type) \
	type* this = malloc(sizeof(type)); \
	this->typename = #type;
#define THIS(type)		struct _ ## type ## * this

#endif