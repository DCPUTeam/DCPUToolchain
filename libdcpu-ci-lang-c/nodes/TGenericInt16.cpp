/**

	File:		TGenericInt16.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TGenericInt Type class.

**/


#include "TGenericInt16.h"

uint16_t TGenericInt16::getWordSize(AsmGenerator& context)
{
	// any 16 bit type has word size 1
	return 1;
}

/* copy */
// direct copy via registers
AsmBlock* TGenericInt16::copyValue(AsmGenerator& context, char from, char to)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << to << ", " << from << std::endl;
	return block;
}
// indirect copy given references (copies values)
AsmBlock* TGenericInt16::copyByRef(AsmGenerator& context, char fromRef, char toRef)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET [" << toRef << "], [" << fromRef << "]" << std::endl;
	return block;
}


// saves value in "from" register into the reference
AsmBlock*  TGenericInt16::saveToRef(AsmGenerator& context, char from, char toRef)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET [" << toRef << "], " << from << std::endl;
	return block;
}

// load from a reference into a value
AsmBlock*  TGenericInt16::loadFromRef(AsmGenerator& context, char fromRef, char to)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << to << ", [" << fromRef << "]" << std::endl;
	return block;
}


AsmBlock* TGenericInt16::setToZero(AsmGenerator& context, char toRef)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET [" << toRef << "], 0x0" << std::endl;
	return block;
}

/*************/
/* stack ops */
/*************/

AsmBlock* TGenericInt16::pushStack(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PUSH, " << a << std::endl;
	return block;
}


// FIXME do i need this? virtual AsmBlock* pushStackByRef(char a);
AsmBlock* TGenericInt16::popStack(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackReturn(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackClean(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PEEK, 0" << std::endl;
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackCleanReturn(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << a << ", PEEK" << std::endl;
	*block <<	"	SET PEEK, 0" << std::endl;
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}



/* binary operators */


AsmBlock* TGenericInt16::add(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::sub(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SUB " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::band(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	AND " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bor(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	BOR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bxor(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	XOR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::shl(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SHL " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::shr(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SHR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::land(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// logical normalization to 0x0=false, 0x1=true
	*block <<	"	IFN " << a << ", 0x0" << std::endl;
	*block <<	"		SET " << a << ", 0x1" << std::endl;
	*block <<	"	IFN " << b << ", 0x0" << std::endl;
	*block <<	"		SET " << b << ", 0x1" << std::endl;
	// bitwise operation
	*block <<	"	AND " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::lor(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// logical normalization to 0x0=false, 0x1=true
	*block <<	"	IFN " << a << ", 0x0" << std::endl;
	*block <<	"		SET " << a << ", 0x1" << std::endl;
	*block <<	"	IFN " << b << ", 0x0" << std::endl;
	*block <<	"		SET " << b << ", 0x1" << std::endl;
	// bitwise operation
	*block <<	"	BOR " << a << ", " << b << std::endl;
	return block;
}



/* unary operators */


AsmBlock* TGenericInt16::plus(AsmGenerator& context, char a)
{
	// TODO integer promotion
	AsmBlock* block = new AsmBlock();
	return block;
}

// FIXME context dependent stack clear with 0

AsmBlock* TGenericInt16::minus(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	// A = 0 - A
	*block <<	"	SET PUSH, " << a << std::endl;
	*block <<	"	SET " << a << ", 0x0" << std::endl;
	*block <<	"	SUB " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bnot(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PUSH, " << a << std::endl;
	*block <<	"	SET " << a << ", 0xffff" << std::endl;
	*block <<	"	XOR " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::lnot(AsmGenerator& context, char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	IFN " << a << ", 0x0" << std::endl;
	*block <<	"		SET " << a << ", 0x1" << std::endl;
	*block <<	"	XOR " << a << ", 0x1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::inc(AsmGenerator& context, char ref)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD [" << ref << "], 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::dec(AsmGenerator& context, char ref)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SUB [" << ref << "], 1" << std::endl;
	return block;
}



/* comparison operators */

// FIXME context dependent stack clear with 0

AsmBlock* TGenericInt16::eq(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFE " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::neq(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFN " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

