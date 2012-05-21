/**

	File:		NInteger.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NInteger AST class.

**/

#ifndef __DCPU_COMP_NODES_INTEGER_H
#define __DCPU_COMP_NODES_INTEGER_H

#include "NExpression.h"
#include "NType.h"

class NInteger : public NExpression
{
	protected:
		NInteger(std::string name) : value(0), NExpression(name) { } // for NGetSizeFunction.
	public:
		long long value;
		NInteger(long long value) : value(value), NExpression("integer") { }
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType* getExpressionType(AsmGenerator& context);

		// General types for integers.
		static NType& CharType;
		static NType& ShortType;
		static NType& IntegerType;
		static NType& LongType;
};

#endif
