/**

	File:		NSwitchStatement.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the NSwitchStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_SWITCH_STATEMENT_H
#define __DCPU_COMP_NODES_SWITCH_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"

class NSwitchStatement : public NStatement
{
	public:
		NExpression& eval;
		NBlock& innerBlock;
		NSwitchStatement(NExpression& eval, NBlock& innerBlock) :
			eval(eval), innerBlock(innerBlock), NStatement("switch") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
