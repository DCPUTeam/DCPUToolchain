/**

	File:			NInteger.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the NInteger AST class.

**/

#ifndef __DCPU_COMP_NODES_INTEGER_H
#define __DCPU_COMP_NODES_INTEGER_H

#include "NExpression.h"

class NInteger : public NExpression
{
protected:
    NInteger(std::string name) : value(0), NExpression(name) { } // for NGetSizeFunction.
public:
    long long value;
    NInteger(long long value) : value(value), NExpression("integer") { }
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual NType& getExpressionType(AsmGenerator& context);
	
	// Specific types for integers.
	static NType& Int8Type;
	static NType& Int16Type;
	static NType& Int32Type;
	static NType& Int64Type;
	static NType& UInt8Type;
	static NType& UInt16Type;
	static NType& UInt32Type;
	static NType& UInt64Type;

	// General types for integers.
	static NType& CharType;
	static NType& ByteType;
	static NType& ShortType;
	static NType& IntegerType;
	static NType& LongType;
};

#endif