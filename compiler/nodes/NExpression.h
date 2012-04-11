/**

	File:			NExpression.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NExpression AST class.

**/

#ifndef __DCPU_COMP_NODES_EXPRESSION_H
#define __DCPU_COMP_NODES_EXPRESSION_H

class NType;
class NExpression;

#include "Node.h"

class NExpression : public Node
{
protected:
	NExpression(std::string type) : Node("expression-" + type) { }
public:
	virtual NType& getExpressionType(AsmGenerator& context) = 0;
};

#endif