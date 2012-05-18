/**

	File:		IType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes
			Patrick Flick

	Description:	Declares the IType AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_TYPE_H
#define __DCPU_COMP_INTERFACES_TYPE_H

#include <stdint.h>
#include <string>
#include "../CompilerException.h"
#include "../AsmGenerator.h"


#define INTERNAL_TYPE_EXCEPTION(_msg)					\
	{								\
		throw new CompilerException(0, "<internal>", 		\
		"Unable to " _msg " unspecified type (internal error)."); \
	}

class IType
{
	public:
		virtual uint16_t getWordSize(AsmGenerator& context)
			INTERNAL_TYPE_EXCEPTION("get word-size of")
		
		virtual std::string getName() const INTERNAL_TYPE_EXCEPTION("get name of")
		virtual std::string getInternalName() const INTERNAL_TYPE_EXCEPTION("get internal name of")
		
		virtual bool isBasicType() const INTERNAL_TYPE_EXCEPTION("get type info of")
		
		virtual bool implicitCastable(const IType* toType) INTERNAL_TYPE_EXCEPTION("get cast info of")
		virtual bool explicitCastable(const IType* toType) INTERNAL_TYPE_EXCEPTION("get cast info of")
		
		virtual AsmBlock* implicitCast(const IType* toType, char a) INTERNAL_TYPE_EXCEPTION("cast")
		virtual AsmBlock* explicitCast(const IType* toType, char a) INTERNAL_TYPE_EXCEPTION("cast")
		
		/* copy */
		// direct copy via registers
		virtual AsmBlock*  copyValue(char from, char to) INTERNAL_TYPE_EXCEPTION("copy value of")
		// indirect copy given references (copies values)
		virtual AsmBlock*  copyByRef(char fromRef, char toRef) INTERNAL_TYPE_EXCEPTION("copy value of")
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(char from, char toRef) INTERNAL_TYPE_EXCEPTION("copy value of")
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(char fromRef, char to) INTERNAL_TYPE_EXCEPTION("copy value of")
		
		/* stack ops */
		virtual AsmBlock*  pushStack(char a) INTERNAL_TYPE_EXCEPTION("push")
		// FIXME do i need this? virtual void pushStackByRef(char a);
		virtual AsmBlock*  popStack() INTERNAL_TYPE_EXCEPTION("pop")
		virtual AsmBlock*  popStackReturn(char a) INTERNAL_TYPE_EXCEPTION("pop")
		// for debugging, cleaning the stack while pop
		virtual AsmBlock*  popStackClean() INTERNAL_TYPE_EXCEPTION("pop")
		virtual AsmBlock*  popStackCleanReturn(char a) INTERNAL_TYPE_EXCEPTION("pop")
		
		/* binary operators */
		virtual AsmBlock*  add(char a, char b) INTERNAL_TYPE_EXCEPTION("add")
		virtual AsmBlock*  sub(char a, char b) INTERNAL_TYPE_EXCEPTION("sub")
		virtual AsmBlock*  mul(char a, char b) INTERNAL_TYPE_EXCEPTION("mul")
		virtual AsmBlock*  div(char a, char b) INTERNAL_TYPE_EXCEPTION("div")
		virtual AsmBlock*  mod(char a, char b) INTERNAL_TYPE_EXCEPTION("mod")
		virtual AsmBlock*  band(char a, char b) INTERNAL_TYPE_EXCEPTION("band")
		virtual AsmBlock*  bor(char a, char b) INTERNAL_TYPE_EXCEPTION("bor")
		virtual AsmBlock*  bxor(char a, char b) INTERNAL_TYPE_EXCEPTION("bxor")
		virtual AsmBlock*  shl(char a, char b) INTERNAL_TYPE_EXCEPTION("shl")
		virtual AsmBlock*  shr(char a, char b) INTERNAL_TYPE_EXCEPTION("shr")
		virtual AsmBlock*  land(char a, char b) INTERNAL_TYPE_EXCEPTION("land")
		virtual AsmBlock*  lor(char a, char b) INTERNAL_TYPE_EXCEPTION("lor")
		
		/* unary operators */
		virtual AsmBlock*  plus(char a) INTERNAL_TYPE_EXCEPTION("use unary plus on")
		virtual AsmBlock*  minus(char a) INTERNAL_TYPE_EXCEPTION("invert")
		virtual AsmBlock*  bnot(char a) INTERNAL_TYPE_EXCEPTION("bitwise invert")
		virtual AsmBlock*  lnot(char a) INTERNAL_TYPE_EXCEPTION("logical invert")
		virtual AsmBlock*  inc(char a) INTERNAL_TYPE_EXCEPTION("increase")
		virtual AsmBlock*  dec(char a) INTERNAL_TYPE_EXCEPTION("decrease")
		
		/* comparison operators */
		virtual AsmBlock*  eq(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  neq(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  gt(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  lt(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  ge(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  le(char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
};

#endif
