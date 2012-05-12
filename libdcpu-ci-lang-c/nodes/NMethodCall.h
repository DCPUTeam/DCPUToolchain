/**

	File:		NMethodCall.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NMethodCall AST class.

**/

#ifndef __DCPU_COMP_NODES_METHOD_CALL_H
#define __DCPU_COMP_NODES_METHOD_CALL_H

class AsmBlock;

#include "NExpression.h"
#include "NIdentifier.h"
#include "Lists.h"

class NMethodCall : public NExpression
{
	public:
		const NIdentifier& id;
		ExpressionList arguments;
		NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
			id(id), arguments(arguments), NExpression("methodcall") { }
		NMethodCall(const NIdentifier& id) : id(id), NExpression("methodcall") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType* getExpressionType(AsmGenerator& context);
		virtual std::string calculateSignature(AsmGenerator& context);
};

#endif
