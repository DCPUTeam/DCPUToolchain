/**

	File:		NCharacter.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NCharacter AST class.

**/

#ifndef __DCPU_COMP_NODES_CHARACTER_H
#define __DCPU_COMP_NODES_CHARACTER_H

#include "NExpression.h"

class NCharacter : public NExpression
{
	public:
		std::string value;
		NCharacter(std::string value) : value(value), NExpression("character") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		AsmBlock* compilePostOperators(AsmGenerator& context) {
			return new AsmBlock();
		}
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType& getExpressionType(AsmGenerator& context);
};

#endif
