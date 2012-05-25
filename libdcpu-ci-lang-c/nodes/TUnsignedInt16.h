/**

	File:		TUnsignedInt16.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TUnsignedInt16 Type class.

**/

#ifndef __DCPU_COMP_TYPES_UINT16_H
#define __DCPU_COMP_TYPES_UINT16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>

#include "TGenericInt16.h"

class TUnsignedInt16 : public TGenericInt16
{
	private:


	public:
		virtual std::string getName() const;
		virtual std::string getInternalName() const;

		// inherits most 16 bit generic integer operations
		// only implements signed versions of the non generic operations

		/* cast operations */
		virtual bool implicitCastable(AsmGenerator& context, const IType* toType);
		virtual bool explicitCastable(AsmGenerator& context, const IType* toType);
		virtual AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a);
		virtual AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a);


		/* binary operations (sign dependent) */
		virtual AsmBlock* mul(AsmGenerator& context, char a, char b);
		virtual AsmBlock* div(AsmGenerator& context, char a, char b);
		virtual AsmBlock* mod(AsmGenerator& context, char a, char b);

		/* comparison operators (sign dependent) */
		virtual AsmBlock* gt(AsmGenerator& context, char a, char b);
		virtual AsmBlock* lt(AsmGenerator& context, char a, char b);
		virtual AsmBlock* ge(AsmGenerator& context, char a, char b);
		virtual AsmBlock* le(AsmGenerator& context, char a, char b);
};
#endif

