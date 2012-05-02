/**

	File:		NIfStatement.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NIfStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_IF_STATEMENT_H
#define __DCPU_COMP_NODES_IF_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"

class NIfStatement : public NStatement
{
	public:
		NExpression& eval;
		NBlock& if_true;
		NBlock* if_false;
		NIfStatement(NExpression& eval, NBlock& if_true) :
			eval(eval), if_true(if_true), if_false(NULL), NStatement("if") { }
		NIfStatement(NExpression& eval, NBlock& if_true, NBlock* if_false) :
			eval(eval), if_true(if_true), if_false(if_false), NStatement("if") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
