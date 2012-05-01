/**

	File:           NIdentifier.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes

	Description:    Declares the NIdentifier AST class.

**/

#ifndef __DCPU_COMP_NODES_IDENTIFIER_H
#define __DCPU_COMP_NODES_IDENTIFIER_H

#include "NExpression.h"

class NIdentifier : public NExpression
{
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name), NExpression("identifier") { }
    NIdentifier(const std::string& name, std::string type) : name(name), NExpression("identifier-" + type) { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual IType& getExpressionType(AsmGenerator& context);
};

#endif
