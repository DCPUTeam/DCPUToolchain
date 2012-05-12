/**

	File:		NBlock.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NBlock AST class.

**/

#ifndef __DCPU_COMP_NODES_BLOCK_H
#define __DCPU_COMP_NODES_BLOCK_H

#include "NExpression.h"
#include "Lists.h"

class NBlock : public NExpression
{
	public:
		StatementList statements;
		NBlock() : NExpression("block") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
