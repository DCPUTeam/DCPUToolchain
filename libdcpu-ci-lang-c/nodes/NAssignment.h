/**

	File:			NAssignment.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NAssignment AST class.

**/

#ifndef __DCPU_COMP_NODES_ASSIGNMENT_H
#define __DCPU_COMP_NODES_ASSIGNMENT_H

#include "NExpression.h"

class NAssignment : public NExpression {
public:
    NExpression& lhs;
    NExpression& rhs;
	int op;
    NAssignment(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs), NExpression("assignment") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual IType& getExpressionType(AsmGenerator& context);
};

#endif