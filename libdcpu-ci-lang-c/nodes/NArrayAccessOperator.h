/**

	File:		NArrayAccessOperator.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NArrayAccessOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_ARRAY_ACCESS_OPERATOR_H
#define __DCPU_COMP_NODES_ARRAY_ACCESS_OPERATOR_H

#include "NExpression.h"

class NArrayAccessOperator : public NExpression
{
	public:
		NExpression& exprA;
		NExpression& exprB;
		NArrayAccessOperator(NExpression& exprA, NExpression& exprB) : exprA(exprA), exprB(exprB), NExpression("arrayaccess") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
