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

void TTypedefType::resolveTypedefs(DeclarationList* declList)
{
	TTypedefType::resolveTypedefs((StatementList*) declList);
}

static void resolveTypedefs(AsmGenerator& context, StatementList* declList)
{
	for (StatementList::iterator i = declList->begin(); i != declList->end(); )
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			// pseudo code for typedef resolval
			// 1. if nvd->type is a typedef:
			// 1.a resolve typedef type
			// 1.1 if type->baseDecl is an array declaration?
			// 1.1.1 replace variable declaration with new array declaration (direct replacement)
			// 1.1.2 something like --i, continue (replaced declaration, which is handled again)
			// 1.2 if type->baseDecl is a variable declaration?
			// 1.2.1 replace variable decl with the new typedef decl
			// 1.2.2 something like --i, continue
			if (nvd->type->isTypedef())
			{
				TTypedefType* tdt = (TTypedefType*) nvd->type;
				tdt->resolve(context);
				if (tdt->baseDeclaration->cType == "statement-declaration-variable")
				{
					*i = tdt->baseDeclaration;
					// TODO replay type modifiers (e.g. const)
					continue;
				}
				else if (tdt->baseDeclaration->cType == "statement-declaration-array")
				{
					*i = tdt->baseDeclaration;
					// TODO replay type modifiers (e.g. const)
					continue;
				}
				else
					throw new CompilerException(0, "<internal>", "Invalid typdef declaration '" + this->id.name + "' encountered!");
			}
		}
		else if ((*i)->cType == "statement-declaration-array")
		{
			// pseudo code for typedef resolval
			// 1. if nad->basetype is a typedef:
			// 1.a resolve typedef type
			// 1.1 if type->baseDecl is an array declaration?
			// 1.1.1 merge array declarations:
			// 1.1.2 add dimensions of this decl to the array decl, creating a new array decl
			// 1.1.3 something like --i, continue (replaced declaration, which is handled again)
			// 1.2 if type->baseDecl is a variable declaration?
			// 1.2.1 replace with new array declaration of this type and dimensions from this
			// 	 currently handled array delaration
			// 1.2.2 --i, continue
			
			NArrayDeclaration* nad = (NArrayDeclaration*)(*i);
			if (nad->m_baseType->isTypedef())
			{
				TTypedefType* tdt = (TTypedefType*) nad->m_baseType;
				tdt->resolve(context);
				if (tdt->baseDeclaration->cType == "statement-declaration-variable")
				{
					IType* basetype = tdt->baseType;
					*i = new NArrayDeclaration(basetype, this->id, nad->m_dims, nad->m_initExprList);
					continue;
				}
				else if (tdt->baseDeclaration->cType == "statement-declaration-array")
				{
					// TODO
					continue;
				}
				else
					throw new CompilerException(0, "<internal>", "Invalid typdef declaration '" + this->id.name + "' encountered!");
			}
		}
		i++;
	}

}

void TTypedefType::resolve(AsmGenerator& context)
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
				{
					this->baseType = nvd->type;
					this->baseDeclaration = nvd;
				}
				return;
			}
			else if (decl->cType == "statement-declaration-array")
			{
				NArrayDeclaration* nad = (NArrayDeclaration*) decl;
				if (nad->id.name == this->id.name)
				{
					this->baseType = nad->getPointerType();
					this->baseDeclaration = nad;
				}
				return;
			}
		}

	throw new CompilerException(0, "<internal>", "Unknown type '" + this->id.name + "' encountered!");
}


		uint16_t TTypedefType::getWordSize(AsmGenerator& context)
		{
			return this->baseType->getWordSize(context);
		}

		std::string TTypedefType::getName() const
		{
			return this->baseType->getName();
		}
		std::string TTypedefType::getInternalName() const
		{
			return this->baseType->getInternalName();
		}

		bool TTypedefType::isBasicType() const
		{
			return this->baseType->isBasicType();
		}
		bool TTypedefType::isStruct() const
		{
			return this->baseType->isStruct();
		}
		bool TTypedefType::isTypedef() const
		{
			return true;
		}
		bool TTypedefType::isPointer() const
		{
			return this->baseType->isPointer();
		}
		bool TTypedefType::isConst() const
		{
			return this->baseType->isConst();
		}
		void TTypedefType::setConst()
		{
			this->baseType->setConst();
		}
		bool TTypedefType::isSigned() const
		{
			return this->baseType->isSigned();
		}
		bool TTypedefType::setSigned(bool isSigned)
		{
			return this->baseType->setSigned(isSigned);
		}

		bool TTypedefType::implicitCastable(AsmGenerator& context, const IType* toType)
		{
			return this->baseType->implicitCastable(context, toType);
		}
		bool TTypedefType::explicitCastable(AsmGenerator& context, const IType* toType)
		{
			return this->baseType->explicitCastable(context, toType);
		}

		AsmBlock* TTypedefType::implicitCast(AsmGenerator& context, const IType* toType, char a)
		{
			return this->baseType->implicitCast(context, toType, a);
		}
		AsmBlock* TTypedefType::explicitCast(AsmGenerator& context, const IType* toType, char a)
		{
			return this->baseType->explicitCast(context, toType, a);
		}

		/* copy */
		// direct copy via registers
		AsmBlock*  TTypedefType::copyValue(AsmGenerator& context, char from, char to)
		{
			return this->baseType->copyValue(context, from, to);
		}
		// indirect copy given references (copies values)
		AsmBlock*  TTypedefType::copyByRef(AsmGenerator& context, char fromRef, char toRef)
		{
			return this->baseType->copyByRef(context, fromRef, toRef);
		}
		// saves value in "from" register into the reference
		AsmBlock*  TTypedefType::saveToRef(AsmGenerator& context, char from, char toRef)
		{
			return this->baseType->saveToRef(context, from, toRef);
		}
		// load from a reference into a value
		AsmBlock*  TTypedefType::loadFromRef(AsmGenerator& context, char fromRef, char to)
		{
			return this->baseType->loadFromRef(context, fromRef, to);
		}
		
		// saves type specific 0 value into the reference
		AsmBlock*  TTypedefType::setToZero(AsmGenerator& context, char toRef)
		{
			return this->baseType->setToZero(context, toRef);
		}

		/* stack ops */
		AsmBlock*  TTypedefType::pushStack(AsmGenerator& context, char a)
		{
			return this->baseType->pushStack(context, a);
		}
		
		// FIXME do i need this? virtual void pushStackByRef(char a);
		AsmBlock*  TTypedefType::popStack(AsmGenerator& context)
		{
			return this->baseType->popStack(context);
		}
		
		AsmBlock*  TTypedefType::popStackReturn(AsmGenerator& context, char a)
		{
			return this->baseType->popStackReturn(context, a);
		}
		
		// for debugging, cleaning the stack while pop
		AsmBlock*  TTypedefType::popStackClean(AsmGenerator& context)
		{
			return this->baseType->popStackClean(context);
		}
		AsmBlock*  TTypedefType::popStackCleanReturn(AsmGenerator& context, char a)
		{
			return this->baseType->popStackCleanReturn(context, a);
		}

		/* binary operators */
		AsmBlock*  TTypedefType::add(AsmGenerator& context, char a, char b)
		{
			return this->baseType->add(context, a, b);
		}
		AsmBlock*  TTypedefType::sub(AsmGenerator& context, char a, char b)
		{
			return this->baseType->sub(context, a, b);
		}
		AsmBlock*  TTypedefType::mul(AsmGenerator& context, char a, char b)
		{
			return this->baseType->mul(context, a, b);
		}
		AsmBlock*  TTypedefType::div(AsmGenerator& context, char a, char b)
		{
			return this->baseType->div(context, a, b);
		}
		AsmBlock*  TTypedefType::mod(AsmGenerator& context, char a, char b)
		{
			return this->baseType->mod(context, a, b);
		}
		AsmBlock*  TTypedefType::band(AsmGenerator& context, char a, char b)
		{
			return this->baseType->band(context, a, b);
		}
		AsmBlock*  TTypedefType::bor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->bor(context, a, b);
		}
		AsmBlock*  TTypedefType::bxor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->bxor(context, a, b);
		}
		AsmBlock*  TTypedefType::shl(AsmGenerator& context, char a, char b)
		{
			return this->baseType->shl(context, a, b);
		}
		AsmBlock*  TTypedefType::shr(AsmGenerator& context, char a, char b)
		{
			return this->baseType->shr(context, a, b);
		}
		AsmBlock*  TTypedefType::land(AsmGenerator& context, char a, char b)
		{
			return this->baseType->land(context, a, b);
		}
		AsmBlock*  TTypedefType::lor(AsmGenerator& context, char a, char b)
		{
			return this->baseType->lor(context, a, b);
		}

		/* unary operators */
		AsmBlock*  TTypedefType::plus(AsmGenerator& context, char a)
		{
			return this->baseType->plus(context, a);
		}
		AsmBlock*  TTypedefType::minus(AsmGenerator& context, char a)
		{
			return this->baseType->minus(context, a);
		}
		AsmBlock*  TTypedefType::bnot(AsmGenerator& context, char a)
		{
			return this->baseType->bnot(context, a);
		}
		AsmBlock*  TTypedefType::lnot(AsmGenerator& context, char a)
		{
			return this->baseType->lnot(context, a);
		}

		AsmBlock*  TTypedefType::inc(AsmGenerator& context, char ref)
		{
			return this->baseType->inc(context, ref);
		}
		AsmBlock*  TTypedefType::dec(AsmGenerator& context, char ref)
		{
			return this->baseType->dec(context, ref);
		}

		/* comparison operators */
		AsmBlock*  TTypedefType::eq(AsmGenerator& context, char a, char b)
		{
			return this->baseType->eq(context, a, b);
		}
		AsmBlock*  TTypedefType::neq(AsmGenerator& context, char a, char b)
		{
			return this->baseType->neq(context, a, b);
		}
		AsmBlock*  TTypedefType::gt(AsmGenerator& context, char a, char b)
		{
			return this->baseType->gt(context, a, b);
		}
		AsmBlock*  TTypedefType::lt(AsmGenerator& context, char a, char b)
		{
			return this->baseType->lt(context, a, b);
		}
		AsmBlock*  TTypedefType::ge(AsmGenerator& context, char a, char b)
		{
			return this->baseType->ge(context, a, b);
		}
		AsmBlock*  TTypedefType::le(AsmGenerator& context, char a, char b)
		{
			return this->baseType->le(context, a, b);
		}




