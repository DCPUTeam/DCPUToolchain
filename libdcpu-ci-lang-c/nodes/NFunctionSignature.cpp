/**

	File:		NFunctionSignature.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NFunctionSignature AST class.

**/

#include <AsmGenerator.h>
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


std::string NFunctionSignature::getSignature() {
	return NFunctionSignature::calculateSignature(this->type, this->arguments);
}

StackMap NFunctionSignature::generateStackMap()
{
	StackMap map;

	// Add stack frame data for arguments.
	for (VariableList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
	{
		map.insert(map.end(), StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//	  other argument declarations (hint: check the map to
		//	  see if it already has a match).
	}

	return map;
}
