/**

	File:			NStructureResolutionOperator.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NStructureResolutionOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_STRUCTURE_RESOLUTION_OPERATOR_H
#define __DCPU_COMP_NODES_STRUCTURE_RESOLUTION_OPERATOR_H

#include "NExpression.h"
#include "NIdentifier.h"
#include "NType.h"

class NStructureResolutionOperator : public NExpression
{
public:
    NExpression& lhs;
    NIdentifier& rhs;
	bool isDereference;
    NStructureResolutionOperator(NExpression& lhs, NIdentifier& rhs, bool isDereference) :
        lhs(lhs), rhs(rhs), isDereference(isDereference), NExpression("field") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual IType& getExpressionType(AsmGenerator& context);
};

#endif