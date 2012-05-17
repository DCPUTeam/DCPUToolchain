/**

	File:		IType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the IType AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_TYPE_H
#define __DCPU_COMP_INTERFACES_TYPE_H

#include <stdint.h>
#include "../CompilerException.h"
#include "../AsmGenerator.h"


#define INTERNAL_TYPE_EXCEPTION(MSG)					\
	{								\
		throw new CompilerException(0, "<internal>", 		\
		"Unable to MSG unspecified type (internal error).");	\
	}

class IType
{
	public:
		virtual uint16_t getWordSize(AsmGenerator& context)
			INTERNAL_TYPE_EXCEPTION(get word-size of)
		
		/*TODO
		(11:08:52 PM) hachque: also add
		(11:09:07 PM) hachque: bool implicitCast(const IType& from);
		(11:09:13 PM) r4d2: yes
		(11:09:16 PM) hachque: and bool explicitCast(const IType& from);
		*/
		
		/* copy */
		// direct copy via registers
		virtual AsmBlock*  copyValue(char from, char to) INTERNAL_TYPE_EXCEPTION(copy value of)
		// indirect copy given references (copies values)
		virtual AsmBlock*  copyByRef(char fromRef, char toRef) INTERNAL_TYPE_EXCEPTION(copy value of)
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(char from, char toRef) INTERNAL_TYPE_EXCEPTION(copy value of)
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(char fromRef, char to) INTERNAL_TYPE_EXCEPTION(copy value of)
		
		/* stack ops */
		virtual AsmBlock*  pushStack(char a) INTERNAL_TYPE_EXCEPTION(push)
		// FIXME do i need this? virtual void pushStackByRef(char a);
		virtual AsmBlock*  popStack() INTERNAL_TYPE_EXCEPTION(pop)
		virtual AsmBlock*  popStackReturn(char a) INTERNAL_TYPE_EXCEPTION(pop)
		// for debugging, cleaning the stack while pop
		virtual AsmBlock*  popStackClean() INTERNAL_TYPE_EXCEPTION(pop)
		virtual AsmBlock*  popStackCleanReturn(char a) INTERNAL_TYPE_EXCEPTION(pop)
		
		/* binary operators */
		virtual AsmBlock*  add(char a, char b) INTERNAL_TYPE_EXCEPTION(add)
		virtual AsmBlock*  sub(char a, char b) INTERNAL_TYPE_EXCEPTION(sub)
		virtual AsmBlock*  mul(char a, char b) INTERNAL_TYPE_EXCEPTION(mul)
		virtual AsmBlock*  div(char a, char b) INTERNAL_TYPE_EXCEPTION(div)
		virtual AsmBlock*  mod(char a, char b) INTERNAL_TYPE_EXCEPTION(mod)
		virtual AsmBlock*  band(char a, char b) INTERNAL_TYPE_EXCEPTION(band)
		virtual AsmBlock*  bor(char a, char b) INTERNAL_TYPE_EXCEPTION(bor)
		virtual AsmBlock*  bxor(char a, char b) INTERNAL_TYPE_EXCEPTION(bxor)
		virtual AsmBlock*  shl(char a, char b) INTERNAL_TYPE_EXCEPTION(shl)
		virtual AsmBlock*  shr(char a, char b) INTERNAL_TYPE_EXCEPTION(shr)
		virtual AsmBlock*  land(char a, char b) INTERNAL_TYPE_EXCEPTION(land)
		virtual AsmBlock*  lor(char a, char b) INTERNAL_TYPE_EXCEPTION(lor)
		
		/* unary operators */
		virtual AsmBlock*  plus(char a) INTERNAL_TYPE_EXCEPTION(use unary plus on)
		virtual AsmBlock*  minus(char a) INTERNAL_TYPE_EXCEPTION(invert)
		virtual AsmBlock*  bnot(char a) INTERNAL_TYPE_EXCEPTION(bitwise invert)
		virtual AsmBlock*  lnot(char a) INTERNAL_TYPE_EXCEPTION(logical invert)
		virtual AsmBlock*  inc(char a) INTERNAL_TYPE_EXCEPTION(increase)
		virtual AsmBlock*  dec(char a) INTERNAL_TYPE_EXCEPTION(decrease)
		
		/* comparison operators */
		virtual AsmBlock*  eq(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
		virtual AsmBlock*  neq(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
		virtual AsmBlock*  gt(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
		virtual AsmBlock*  lt(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
		virtual AsmBlock*  ge(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
		virtual AsmBlock*  le(char a, char b) INTERNAL_TYPE_EXCEPTION(compare)
};

#endif
