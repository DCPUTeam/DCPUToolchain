/**

	File:			NDeclaration.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_DECLARATION_H
#define __DCPU_COMP_NODES_DECLARATION_H

#include "NStatement.h"

class NDeclaration : public NStatement
{
protected:
	NDeclaration(std::string type) : NStatement("declaration-" + type) { }
};

#endif