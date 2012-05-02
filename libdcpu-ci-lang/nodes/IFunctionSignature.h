/**

	File:		IFunctionSignature.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the IFunctionSignature AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_FUNCTION_SIGNATURE_H
#define __DCPU_COMP_INTERFACES_FUNCTION_SIGNATURE_H

#include "../StackMap.h"

class IFunctionSignature
{
	public:
		virtual StackMap generateStackMap() = 0;
};

#endif
