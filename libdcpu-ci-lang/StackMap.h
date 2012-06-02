/**

	File:		StackMap.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the StackMap typedef.

**/

#ifndef __DCPU_LIBDCC_STACKMAP_H
#define __DCPU_LIBDCC_STACKMAP_H

struct IType; // Future declaration.

struct StackPair
{
	bstring first;
	IType* second;
};

#endif
