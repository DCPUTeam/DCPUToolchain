/**

	File:			NAssemblyStatement.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NAssemblyStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_ASSEMBLY_STATEMENT_H
#define __DCPU_COMP_NODES_ASSEMBLY_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NAssemblyStatement : public NStatement {
public:
    std::string asmcode;
    NAssemblyStatement(const std::string& asmcode) :
        asmcode(asmcode), NStatement("assembly") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
};

#endif