/**

	File:		TTypedefType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TTypedefType Type class.

**/

#ifndef __DCPU_COMP_TYPES_TYPEDEFTYPE_H
#define __DCPU_COMP_TYPES_TYPEDEFTYPE_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>
#include "NBinaryOperator.h"

class TTypedefType: public IType
{
	private:

	public:
		IType* baseType;
		NIdentifier& id;

		void resolveType(AsmGenerator& context);

		virtual uint16_t getWordSize(AsmGenerator& context);

		virtual std::string getName() const;
		virtual std::string getInternalName() const;

		virtual bool isBasicType() const;
		virtual bool isStruct() const;
		virtual bool isPointer() const;
		virtual bool isConst() const;
		virtual void setConst();
		virtual bool isSigned() const;
		virtual bool setSigned(bool isSigned);

		virtual bool implicitCastable(AsmGenerator& context, const IType* toType);
		virtual bool explicitCastable(AsmGenerator& context, const IType* toType);

		virtual AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a);
		virtual AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a);

		/* copy */
		// direct copy via registers
		virtual AsmBlock*  copyValue(AsmGenerator& context, char from, char to);
		// indirect copy given references (copies values)
		virtual AsmBlock*  copyByRef(AsmGenerator& context, char fromRef, char toRef);
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(AsmGenerator& context, char from, char toRef);
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(AsmGenerator& context, char fromRef, char to);
		
		// saves type specific 0 value into the reference
		virtual AsmBlock*  setToZero(AsmGenerator& context, char toRef);

		/* stack ops */
		virtual AsmBlock*  pushStack(AsmGenerator& context, char a);
		// FIXME do i need this? virtual void pushStackByRef(char a);
		virtual AsmBlock*  popStack(AsmGenerator& context);
		virtual AsmBlock*  popStackReturn(AsmGenerator& context, char a);
		// for debugging, cleaning the stack while pop
		virtual AsmBlock*  popStackClean(AsmGenerator& context);
		virtual AsmBlock*  popStackCleanReturn(AsmGenerator& context, char a);

		/* binary operators */
		virtual AsmBlock*  add(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  sub(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  mul(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  div(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  mod(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  band(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  bor(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  bxor(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  shl(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  shr(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  land(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  lor(AsmGenerator& context, char a, char b);

		/* unary operators */
		virtual AsmBlock*  plus(AsmGenerator& context, char a);
		virtual AsmBlock*  minus(AsmGenerator& context, char a);
		virtual AsmBlock*  bnot(AsmGenerator& context, char a);
		virtual AsmBlock*  lnot(AsmGenerator& context, char a);

		virtual AsmBlock*  inc(AsmGenerator& context, char ref);
		virtual AsmBlock*  dec(AsmGenerator& context, char ref);

		/* comparison operators */
		virtual AsmBlock*  eq(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  neq(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  gt(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  lt(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  ge(AsmGenerator& context, char a, char b);
		virtual AsmBlock*  le(AsmGenerator& context, char a, char b);

};
#endif

