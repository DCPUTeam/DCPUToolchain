/**

	File:		NSizeOfOperator.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NSizeOfOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_GET_SIZE_FUNCTION_H
#define __DCPU_COMP_NODES_GET_SIZE_FUNCTION_H

#include "NIntegerLiteral.h"
#include "NType.h"

class NSizeOfOperator : public NIntegerLiteral
{
	public:
		IType* value;
		NSizeOfOperator(IType* value) : value(value), NIntegerLiteral("sizeof") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
