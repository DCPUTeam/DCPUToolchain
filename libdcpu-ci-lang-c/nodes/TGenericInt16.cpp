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
AsmBlock* TGenericInt16::copyValue(char from, char to)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << to << ", " << from << std::endl;
	return block;
}
// indirect copy given references (copies values)
AsmBlock* TGenericInt16::copyByRef(char fromRef, char toRef)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET [" << toRef << "], [" << fromRef << "]" << std::endl;
	return block;
}


// saves value in "from" register into the reference
AsmBlock*  TGenericInt16::saveToRef(char from, char toRef)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET [" << toRef << "], " << from << std::endl;
	return block;
}

// load from a reference into a value
AsmBlock*  TGenericInt16::loadFromRef(char fromRef, char to)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << to << ", [" << fromRef << "]" << std::endl;
	return block;
}

/*************/
/* stack ops */
/*************/

AsmBlock* TGenericInt16::pushStack(char a) {
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PUSH, " << a << std::endl;
	return block;
}


// FIXME do i need this? virtual AsmBlock* pushStackByRef(char a);
AsmBlock* TGenericInt16::popStack()
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackReturn(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	POP SP, " << a << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackClean()
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PEEK, 0" << std::endl;
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::popStackCleanReturn(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET " << a << ", PEEK" << std::endl;
	*block <<	"	SET PEEK, 0" << std::endl;
	*block <<	"	ADD SP, 1" << std::endl;
	return block;
}



	/* binary operators */


AsmBlock* TGenericInt16::add(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::sub(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SUB " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::band(char a, char b){
	AsmBlock* block = new AsmBlock();
	*block <<	"	AND " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bor(char a, char b){
	AsmBlock* block = new AsmBlock();
	*block <<	"	BOR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bxor(char a, char b){
	AsmBlock* block = new AsmBlock();
	*block <<	"	XOR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::shl(char a, char b){
	AsmBlock* block = new AsmBlock();
	*block <<	"	SHL " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::shr(char a, char b){
	AsmBlock* block = new AsmBlock();
	*block <<	"	SHR " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TGenericInt16::land(char a, char b)
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

AsmBlock* TGenericInt16::lor(char a, char b)
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
	
	
AsmBlock* TGenericInt16::plus(char a)
{
	// TODO integer promotion
	AsmBlock* block = new AsmBlock();
	return block;
}

// FIXME context dependent stack clear with 0	

AsmBlock* TGenericInt16::minus(char a)
{
	AsmBlock* block = new AsmBlock();
	// A = 0 - A
	*block <<	"	SET PUSH, " << a << std::endl;
	*block <<	"	SET " << a << ", 0x0" << std::endl;
	*block <<	"	SUB " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::bnot(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SET PUSH, " << a << std::endl;
	*block <<	"	SET " << a << ", 0xffff" << std::endl;
	*block <<	"	XOR " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::lnot(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	IFN " << a << ", 0x0" << std::endl;
	*block <<	"		SET " << a << ", 0x1" << std::endl;
	*block <<	"	XOR " << a << ", 0x1" << std::endl;
	return block;
}
		
AsmBlock* TGenericInt16::inc(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD " << a << ", 1" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::dec(char a)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SUB " << a << ", 1" << std::endl;
	return block;
}


		
	/* comparison operators */
	
// FIXME context dependent stack clear with 0	

AsmBlock* TGenericInt16::eq(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFE " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TGenericInt16::neq(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFN " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

