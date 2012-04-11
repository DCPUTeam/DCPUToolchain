/**

	File:			NDereferenceOperator.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NDereferenceOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_DEREFERENCE_OPERATOR_H
#define __DCPU_COMP_NODES_DEREFERENCE_OPERATOR_H

#include "NExpression.h"

class NDereferenceOperator : public NExpression
{
public:
    NExpression& expr;
	NDereferenceOperator(NExpression& expr) : expr(expr), NExpression("dereference") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual NType& getExpressionType(AsmGenerator& context);
};

#endif