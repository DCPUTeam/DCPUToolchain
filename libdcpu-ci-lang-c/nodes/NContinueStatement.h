/**

	File:		NContinueStatement.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the NContinueStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_CONTINUE_STATEMENT_H
#define __DCPU_COMP_NODES_CONTINUE_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NContinueStatement : public NStatement
{
	public:
		NContinueStatement() :
			NStatement("continue") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
