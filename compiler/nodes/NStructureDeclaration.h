/**

	File:			NStructureDeclaration.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NStructureDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_STRUCTURE_DECLARATION_H
#define __DCPU_COMP_NODES_STRUCTURE_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "Lists.h"

class NStructureDeclaration : public NDeclaration
{
public:
	const NIdentifier& id;
	VariableList fields;
	NStructureDeclaration(const NIdentifier& id, const VariableList& fields) :
		id(id), fields(fields), NDeclaration("structure") { };
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual size_t getSize(AsmGenerator& context);
};

#endif
