/**

	File:		NDebugStatement.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NDebugStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_DEBUG_STATEMENT_H
#define __DCPU_COMP_NODES_DEBUG_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NDebugStatement : public NStatement
{
	public:
		const NExpression& result;
		NDebugStatement(const NExpression& result) :
			result(result), NStatement("debug") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
