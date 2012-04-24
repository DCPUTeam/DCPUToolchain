/**

	File:			NWhileStatement.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NWhileStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_WHILE_STATEMENT_H
#define __DCPU_COMP_NODES_WHILE_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"

class NWhileStatement : public NStatement {
public:
    NExpression& eval;
    NBlock& expr;
    NWhileStatement(NExpression& eval, NBlock& expr) :
        eval(eval), expr(expr), NStatement("while") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
};

#endif