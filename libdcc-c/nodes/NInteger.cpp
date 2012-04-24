/**

	File:			NInteger.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NInteger AST class.

**/

#include <asmgen.h>
#include "NInteger.h"

NType internal_CharType("char", 0, false);
NType internal_ByteType("byte", 0, false);
NType internal_ShortType("short", 0, false);
NType internal_IntegerType("int", 0, false);
NType internal_LongType("long", 0, false);
NType internal_Int8Type("int8_t", 0, false);
NType internal_Int16Type("int16_t", 0, false);
NType internal_Int32Type("int32_t", 0, false);
NType internal_Int64Type("int64_t", 0, false);
NType internal_UInt8Type("uint8_t", 0, false);
NType internal_UInt16Type("uint16_t", 0, false);
NType internal_UInt32Type("uint32_t", 0, false);
NType internal_UInt64Type("uint64_t", 0, false);
NType& NInteger::CharType(internal_CharType);
NType& NInteger::ByteType(internal_ByteType);
NType& NInteger::ShortType(internal_ShortType);
NType& NInteger::IntegerType(internal_IntegerType);
NType& NInteger::LongType(internal_LongType);
NType& NInteger::Int8Type(internal_Int8Type);
NType& NInteger::Int16Type(internal_Int16Type);
NType& NInteger::Int32Type(internal_Int32Type);
NType& NInteger::Int64Type(internal_Int64Type);
NType& NInteger::UInt8Type(internal_UInt8Type);
NType& NInteger::UInt16Type(internal_UInt16Type);
NType& NInteger::UInt32Type(internal_UInt32Type);
NType& NInteger::UInt64Type(internal_UInt64Type);

AsmBlock* NInteger::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Load the numeric value in register A.
	*block <<	"	SET A, " << this->value << std::endl;

	return block;
}

AsmBlock* NInteger::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a numeric literal.");
}

IType& NInteger::getExpressionType(AsmGenerator& context)
{
	// All literals are currently unsigned 16-bit integers.
	return NInteger::UInt16Type;
}