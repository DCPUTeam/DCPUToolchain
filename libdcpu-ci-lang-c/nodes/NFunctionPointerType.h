/**

	File:		NFunctionPointerType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NFunctionPointerType AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_POINTER_TYPE_H
#define __DCPU_COMP_NODES_FUNCTION_POINTER_TYPE_H

#include <stdint.h>
#include "NFunctionSignature.h"
#include "NType.h"
#include <nodes/IType.h>
#include "Lists.h"

class NFunctionPointerType : public NType, public NFunctionSignature
{
	public:
		NFunctionPointerType(const IType* returnType, const VariableList& arguments) :
			NType("function", NFunctionSignature::calculateSignature(returnType, arguments), 1, false), NFunctionSignature(returnType, arguments) { }
};

#endif
