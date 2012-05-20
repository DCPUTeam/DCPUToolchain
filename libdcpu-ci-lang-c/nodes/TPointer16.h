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
		/* inherits all Uint16 operations */
		
		TPointer16(IType* pointingTo) : m_pointingTo(pointingTo) { }
		
		IType* getPointerBaseType();
		
		virtual std::string getName() const;
		virtual std::string getInternalName() const;
		
		/* cast operations */
		virtual bool implicitCastable(const IType* toType);
		virtual bool explicitCastable(const IType* toType);
		virtual AsmBlock* implicitCast(const IType* toType, char a);
		virtual AsmBlock* explicitCast(const IType* toType, char a);
};
#endif

