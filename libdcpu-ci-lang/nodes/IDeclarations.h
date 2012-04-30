/**

	File:           IDeclarations.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang

	Authors:        James Rhodes

	Description:    Declares the IDeclarations AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_DECLARATIONS_H
#define __DCPU_COMP_INTERFACES_DECLARATIONS_H

#include <string>
#include "IFunctionDeclaration.h"

class IDeclarations
{
public:
	virtual IFunctionDeclaration* getFunction(std::string name) = 0;
};

#endif
