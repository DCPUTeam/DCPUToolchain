/**

	File:			NFunctionSignature.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NFunctionSignature AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_SIGNATURE_H
#define __DCPU_COMP_NODES_FUNCTION_SIGNATURE_H

#include "NType.h"
#include "Lists.h"

class NFunctionSignature
{
public:
	const NType& type;
	const VariableList arguments;
	NFunctionSignature(const NType& type, const VariableList& arguments)
		: type(type), arguments(arguments) { };
	static std::string calculateSignature(const NType& returnType, const VariableList& arguments);
};

#endif