/**

	File:		NTypedefDeclaration.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the NTypedefDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_TYPEDEF_DECLARATION_H
#define __DCPU_COMP_NODES_TYPEDEF_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NExpression.h"
#include "NType.h"

class NTypedefDeclaration : public NDeclaration
{
	public:
		NDeclaration* decl;
		NTypedefDeclaration(NDeclaration* decl) :
			decl(decl),  NDeclaration("typedef") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
