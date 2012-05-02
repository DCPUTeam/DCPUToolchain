/**

	File:		NExpressionStatement.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NExpressionStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_EXPRESSION_STATEMENT_H
#define __DCPU_COMP_NODES_EXPRESSION_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NExpressionStatement : public NStatement
{
	public:
		NExpression& expression;
		NExpressionStatement(NExpression& expression) :
			expression(expression), NStatement("expression") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
