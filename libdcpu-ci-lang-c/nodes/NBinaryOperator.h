/**

	File:		NBinaryOperator.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NBinaryOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_BINARY_OPERATOR_H
#define __DCPU_COMP_NODES_BINARY_OPERATOR_H

#include "NExpression.h"

class NBinaryOperator : public NExpression
{
	public:
		NExpression& lhs;
		NExpression& rhs;
		int op;
		NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
			lhs(lhs), op(op), rhs(rhs), NExpression("binaryop") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
