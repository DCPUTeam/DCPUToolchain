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
		throw new CompilerException(0, "<internal>",		\
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
		virtual bool isStruct() const INTERNAL_TYPE_EXCEPTION("get type info of")
		virtual bool isPointer() const INTERNAL_TYPE_EXCEPTION("get type info of")
		virtual bool isConst() const INTERNAL_TYPE_EXCEPTION("get type info of")
		virtual void setConst() INTERNAL_TYPE_EXCEPTION("get type info of")
		virtual bool isSigned() const INTERNAL_TYPE_EXCEPTION("get type info of")
		virtual bool setSigned(bool isSigned) INTERNAL_TYPE_EXCEPTION("get type info of")

		virtual bool implicitCastable(AsmGenerator& context, const IType* toType) INTERNAL_TYPE_EXCEPTION("get cast info of")
		virtual bool explicitCastable(AsmGenerator& context, const IType* toType) INTERNAL_TYPE_EXCEPTION("get cast info of")

		virtual AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a) INTERNAL_TYPE_EXCEPTION("cast")
		virtual AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a) INTERNAL_TYPE_EXCEPTION("cast")

		/* copy */
		// direct copy via registers
		virtual AsmBlock*  copyValue(AsmGenerator& context, char from, char to) INTERNAL_TYPE_EXCEPTION("copy value of")
		// indirect copy given references (copies values)
		virtual AsmBlock*  copyByRef(AsmGenerator& context, char fromRef, char toRef) INTERNAL_TYPE_EXCEPTION("copy value of")
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(AsmGenerator& context, char from, char toRef) INTERNAL_TYPE_EXCEPTION("copy value of")
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(AsmGenerator& context, char fromRef, char to) INTERNAL_TYPE_EXCEPTION("copy value of")
		
		// saves type specific 0 value into the reference
		virtual AsmBlock*  setToZero(AsmGenerator& context, char toRef) INTERNAL_TYPE_EXCEPTION("copy value of")

		/* stack ops */
		virtual AsmBlock*  pushStack(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("push")
		// FIXME do i need this? virtual void pushStackByRef(char a);
		virtual AsmBlock*  popStack(AsmGenerator& context) INTERNAL_TYPE_EXCEPTION("pop")
		virtual AsmBlock*  popStackReturn(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("pop")
		// for debugging, cleaning the stack while pop
		virtual AsmBlock*  popStackClean(AsmGenerator& context) INTERNAL_TYPE_EXCEPTION("pop")
		virtual AsmBlock*  popStackCleanReturn(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("pop")

		/* binary operators */
		virtual AsmBlock*  add(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("add")
		virtual AsmBlock*  sub(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("sub")
		virtual AsmBlock*  mul(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("mul")
		virtual AsmBlock*  div(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("div")
		virtual AsmBlock*  mod(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("mod")
		virtual AsmBlock*  band(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("band")
		virtual AsmBlock*  bor(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("bor")
		virtual AsmBlock*  bxor(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("bxor")
		virtual AsmBlock*  shl(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("shl")
		virtual AsmBlock*  shr(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("shr")
		virtual AsmBlock*  land(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("land")
		virtual AsmBlock*  lor(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("lor")

		/* unary operators */
		virtual AsmBlock*  plus(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("use unary plus on")
		virtual AsmBlock*  minus(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("invert")
		virtual AsmBlock*  bnot(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("bitwise invert")
		virtual AsmBlock*  lnot(AsmGenerator& context, char a) INTERNAL_TYPE_EXCEPTION("logical invert")

		virtual AsmBlock*  inc(AsmGenerator& context, char ref) INTERNAL_TYPE_EXCEPTION("increase")
		virtual AsmBlock*  dec(AsmGenerator& context, char ref) INTERNAL_TYPE_EXCEPTION("decrease")

		/* comparison operators */
		virtual AsmBlock*  eq(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  neq(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  gt(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  lt(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  ge(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
		virtual AsmBlock*  le(AsmGenerator& context, char a, char b) INTERNAL_TYPE_EXCEPTION("compare")
};

#endif
