/**

	File:		TPointer16.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TPointer Type class.

**/


#ifndef __DCPU_COMP_TYPES_POINTER16_H
#define __DCPU_COMP_TYPES_POINTER16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>

#include "TUint16.h"

class TPointer16 : public TUint16 
{
	private:
		IType* m_pointingTo;

	public:
		/* inherit copy, ref, push, pop, and compare ops from Uint16 */
		
		TPointer16(IType* pointingTo) : m_pointingTo(pointingTo) { }
		
		IType* getPointerBaseType();
		
		static AsmBlock* compileBinaryOperator(NBinaryOperator* binopNode, AsmGenerator& context);
		
		bool isBasicType() const;
		bool isPointer() const;
		
		virtual std::string getName() const;
		virtual std::string getInternalName() const;
		
		/* cast operations */
		virtual bool implicitCastable(AsmGenerator& context, const IType* toType);
		virtual bool explicitCastable(AsmGenerator& context, const IType* toType);
		virtual AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a);
		virtual AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a);
		
		
		/* binary operators */
		virtual AsmBlock* add(AsmGenerator& context, char a, char b);
		virtual AsmBlock* sub(AsmGenerator& context, char a, char b);

		
		/* illegal binary ops */
		virtual AsmBlock* band(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* bor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* bxor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* shl(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* shr(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* land(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* lor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
				
		virtual AsmBlock* mul(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* div(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* mod(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		
		/* unary operators */
		virtual AsmBlock* inc(AsmGenerator& context, char a);
		virtual AsmBlock* dec(AsmGenerator& context, char a);
		
		/* illegal unary operators */
		virtual AsmBlock* plus(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* minus(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* bnot(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}
		virtual AsmBlock* lnot(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for pointers (internal error).");
		}

};
#endif

