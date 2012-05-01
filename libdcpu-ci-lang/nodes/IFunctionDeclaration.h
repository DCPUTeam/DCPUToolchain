/**

	File:           IFunctionDeclaration.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang

	Authors:        James Rhodes

	Description:    Declares the IFunctionDeclaration AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_FUNCTION_DECLARATION_H
#define __DCPU_COMP_INTERFACES_FUNCTION_DECLARATION_H

#include "../StackMap.h"

class IFunctionDeclaration
{
public:
	virtual StackMap generateStackMap() = 0;
	virtual IType* getPointerType() = 0;
};

#endif
