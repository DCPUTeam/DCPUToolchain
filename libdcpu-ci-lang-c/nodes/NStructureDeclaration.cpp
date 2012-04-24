/**

	File:			NStructureDeclaration.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NStructureDeclaration AST class.

**/

#include <asmgen.h>
#include "NStructureDeclaration.h"

AsmBlock* NStructureDeclaration::compile(AsmGenerator& context)
{
	// There is nothing to do for a structure declaration as
	// everything is handled when we first use the structure.
	return NULL;
}

AsmBlock* NStructureDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a structure declaration.");
}

size_t NStructureDeclaration::getSize(AsmGenerator& context)
{
	// Return the size of each of the fields.
	size_t s = 0;
	for (VariableList::iterator i = this->fields.begin(); i != this->fields.end(); i++)
		s += (*i)->type.getWordSize(context);
	return s;
}