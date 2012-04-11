/**

	File:			NDeclarations.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NDeclarations AST class.

**/

#ifndef __DCPU_COMP_NODES_DECLARATIONS_H
#define __DCPU_COMP_NODES_DECLARATIONS_H

#include "Node.h"
#include "Lists.h"

class NDeclarations : public Node
{
public:
	DeclarationList definitions;
	NDeclarations() : Node("declarations") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
};

#endif