/**

	File:			NFunctionDeclaration.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NFunctionDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_DECLARATION_H
#define __DCPU_COMP_NODES_FUNCTION_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NBlock.h"
#include "NType.h"

class NFunctionDeclaration : public NDeclaration
{
public:
	const NType& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(const NType& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) :
		type(type), id(id), block(block), arguments(arguments), NDeclaration("function") { };
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
};

#endif