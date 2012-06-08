/**

	File:		TTypedefType.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TTypedefType Type class.

**/
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "TTypedefType.h"
#include "../parser.hpp"


void resolveType(AsmGenerator& context)
{
	if (this->baseType != NULL)
		return;

	// Search AST for typedef nodes (only on global scale)
	NDeclarations* decls = (NDeclarations*)context.m_RootNode;

	for (DeclarationList::iterator i = decls->definitions.begin(); i != decls->definitions.end(); i++)
		if ((*i)->cType == "statement-declaration-typedef")
		{
			NDeclaration* decl = ((NTypedefDeclaration*)*i)->decl;
			if (decl->cType == "statement-declaration-variable")
			{
				NVariableDeclaration* nvd = (NVariableDeclaration*) decl;
				if (nvd->id.name == this->id.name)
					this->baseType = nvd->type;
				return;
			}
			else if (decl->cType == "statement-declaration-array")
			{
				NArrayDeclaration* nad = (NArrayDeclaration*) decl;
				if (nad->id.name == this->id.name)
					this->baseType = nvd->getPointerType();
				return;
			}
		}

	throw new CompilerException(0, "<internal>", "Unknown struct type " + this->m_name + " encountered!");
}


		uint16_t getWordSize(AsmGenerator& context)
		{
			return this->baseType->getWordSize(context);
		}

		std::string getName() const
		{
			return this->baseType->getName();
		}
		std::string getInternalName() const
		{
			return this->baseType->getInternalName();
		}

		bool isBasicType() const
		{
			return this->baseType->isBasicType();
		}
		bool isStruct() const
		{
			return this->baseType->isStruct();
		}
		bool isPointer() const
		{
			return this->baseType->isPointer();
		}
		bool isConst() const
		{
			return this->baseType->isConst();
		}
		void setConst()
		{
			this->baseType->setConst();
		}
		bool isSigned() const
		{
			return this->baseType->isSigned();
		}
		bool setSigned(bool isSigned)
		{
			return this->baseType->setSigned();
		}

		bool implicitCastable(AsmGenerator& context, const IType* toType)
		{
			return this->baseType->implicitCastable(context, toType);
		}
		bool explicitCastable(AsmGenerator& context, const IType* toType)
		{
			return this->baseType->explicitCastable(context, toType);
		}

		AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a)
		{
			return this->baseType->implicitCast(context, toType, a);
		}
		AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a)
		{
			return this->baseType->explicitCast(context, toType, a);
		}

		/* copy */
		// direct copy via registers
		AsmBlock*  copyValue(AsmGenerator& context, char from, char to)
		{
			return this->baseType->copyValue(context, from, to);
		}
		// indirect copy given references (copies values)
		AsmBlock*  copyByRef(AsmGenerator& context, char fromRef, char toRef)
		{
			return this->baseType->copyByRef(context, fromRef, toRef);
		}
		// saves value in "from" register into the reference
		AsmBlock*  saveToRef(AsmGenerator& context, char from, char toRef)
		{
			return this->baseType->saveToRef(context, from, toRef);
		}
		// load from a reference into a value
		AsmBlock*  loadFromRef(AsmGenerator& context, char fromRef, char to)
		{
			return this->baseType->loadFromRef(context, fromRef, to);
		}
		
		// saves type specific 0 value into the reference
		AsmBlock*  setToZero(AsmGenerator& context, char toRef)
		{
			return this->baseType->setToZero(context, toRef);
		}

		/* stack ops */
		AsmBlock*  pushStack(AsmGenerator& context, char a)
		{
			return this->baseType->pushStack(context, a);
		}
		
		// FIXME do i need this? virtual void pushStackByRef(char a);
		AsmBlock*  popStack(AsmGenerator& context)
		{
			return this->baseType->popStack(context);
		}
		
		AsmBlock*  popStackReturn(AsmGenerator& context, char a)
		{
			return this->baseType->popStackReturn(context, a);
		}
		
		// for debugging, cleaning the stack while pop
		AsmBlock*  popStackClean(AsmGenerator& context)
		{
			return this->baseType->popStackClean(context);
		}
		AsmBlock*  popStackCleanReturn(AsmGenerator& context, char a)
		{
			return this->baseType->popStackCleanReturn(context, a);
		}

		/* binary operators */
		AsmBlock*  add(AsmGenerator& context, char a, char b)
		{
			return this->baseType->add(context, a, b);
		}
		AsmBlock*  sub(AsmGenerator& context, char a, char b)
		{
			return this->baseType->sub(context, a, b);
		}
		AsmBlock*  mul(AsmGenerator& context, char a, char b)
		{
			return this->baseType->mul(context, a, b);
		}
		AsmBlock*  div(AsmGenerator& context, char a, char b)
		{
			return this->baseType->div(context, a, b);
		}
		AsmBlock*  mod(AsmGenerator& context, char a, char b)
		{
			return this->baseType->mod(context, a, b);
		}
		AsmBlock*  band(AsmGenerator& context, char a, char b)
		{
			return this->baseType->band(context, a, b);
		}
		AsmBlock*  bor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->bor(context, a, b);
		}
		AsmBlock*  bxor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->bxor(context, a, b);
		}
		AsmBlock*  shl(AsmGenerator& context, char a, char b)
		{
			return this->baseType->shl(context, a, b);
		}
		AsmBlock*  shr(AsmGenerator& context, char a, char b)
		{
			return this->baseType->shr(context, a, b);
		}
		AsmBlock*  land(AsmGenerator& context, char a, char b)
		{
			return this->baseType->land(context, a, b);
		}
		AsmBlock*  lor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->lor(context, a, b);
		}

		/* unary operators */
		AsmBlock*  plus(AsmGenerator& context, char a)
		{
			return this->baseType->plus(context, a);
		}
		AsmBlock*  minus(AsmGenerator& context, char a)
		{
			return this->baseType->minus(context, a);
		}
		AsmBlock*  bnot(AsmGenerator& context, char a)
		{
			return this->baseType->bnot(context, a);
		}
		AsmBlock*  lnot(AsmGenerator& context, char a)
		{
			return this->baseType->lnot(context, a);
		}

		AsmBlock*  inc(AsmGenerator& context, char ref)
		{
			return this->baseType->inc(context, ref);
		}
		AsmBlock*  dec(AsmGenerator& context, char ref)
		{
			return this->baseType->dec(context, ref);
		}

		/* comparison operators */
		AsmBlock*  eq(AsmGenerator& context, char a, char b)
		{
			return this->baseType->eq(context, a, b);
		}
		AsmBlock*  neq(AsmGenerator& context, char a, char b)
		{
			return this->baseType->neq(context, a, b);
		}
		AsmBlock*  gt(AsmGenerator& context, char a, char b)
		{
			return this->baseType->gt(context, a, b);
		}
		AsmBlock*  lt(AsmGenerator& context, char a, char b)
		{
			return this->baseType->lt((context, a, b);
		}
		AsmBlock*  ge(AsmGenerator& context, char a, char b)
		{
			return this->baseType->ge(context, a, b);
		}
		AsmBlock*  le(AsmGenerator& context, char a, char b)
		{
			return this->baseType->le(context, a, b);
		}




