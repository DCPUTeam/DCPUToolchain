/**

	File:			NFunctionSignature.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NFunctionSignature AST class.

**/

#include "../asmgen.h"
#include "NFunctionSignature.h"
#include "Lists.h"

std::string NFunctionSignature::calculateSignature(const NType& returnType, const VariableList& arguments)
{
	std::string name = "fp-(";
	name += returnType.name + "-";
	for (VariableList::const_iterator i = arguments.begin(); i != arguments.end(); i++)
		name += (*i)->type.name + ",";
	name += ")";
	return name;
}