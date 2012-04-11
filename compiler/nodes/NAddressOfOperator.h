/**

	File:			NAddressOfOperator.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NAddressOfOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_ADDRESSOF_OPERATOR_H
#define __DCPU_COMP_NODES_ADDRESSOF_OPERATOR_H

#include "NExpression.h"

class NAddressOfOperator : public NExpression
{
public:
    NExpression& expr;
	NAddressOfOperator(NExpression& expr) : expr(expr), NExpression("addressof") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual NType& getExpressionType(AsmGenerator& context);
};

#endif