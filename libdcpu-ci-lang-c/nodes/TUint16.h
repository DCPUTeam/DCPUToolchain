/**

	File:		TUint16.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TUint16 Type class.

**/

#ifndef __DCPU_COMP_TYPES_UINT16_H
#define __DCPU_COMP_TYPES_UINT16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>

#include "TGenericInt16.h"

class TUint16 : public TGenericInt16
{
	private:
		

	public:
		virtual std::string getName() const;
		virtual std::string getInternalName() const;
	
		// inherits most 16 bit generic integer operations
		// only implements signed versions of the non generic operations
			
		/* cast operations */
		virtual bool implicitCastable(const IType* toType);
		virtual bool explicitCastable(const IType* toType);
		virtual AsmBlock* implicitCast(const IType* toType, char a);
		virtual AsmBlock* explicitCast(const IType* toType, char a);
			
			
		/* binary operations (sign dependent) */
		virtual AsmBlock* mul(char a, char b);
		virtual AsmBlock* div(char a, char b);
		virtual AsmBlock* mod(char a, char b);
		
		/* comparison operators (sign dependent) */
		virtual AsmBlock* gt(char a, char b);
		virtual AsmBlock* lt(char a, char b);
		virtual AsmBlock* ge(char a, char b);
		virtual AsmBlock* le(char a, char b);
};
#endif

