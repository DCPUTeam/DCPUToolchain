/**

	File:		NExpression.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NExpression AST class.

**/

#ifndef __DCPU_COMP_NODES_EXPRESSION_H
#define __DCPU_COMP_NODES_EXPRESSION_H

class IType;
class NExpression;

#include "Node.h"

class NExpression : public Node
{
	protected:
		NExpression(std::string type) : Node("expression-" + type) { }
	public:
		virtual IType& getExpressionType(AsmGenerator& context) = 0;
};

#endif
