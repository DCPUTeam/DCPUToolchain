/**

	File:		TInt16.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TInt Type class.

**/

#ifndef __DCPU_COMP_TYPES_INT16_H
#define __DCPU_COMP_TYPES_INT16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <string>
#include <nodes/IType.h>
#include "TGenericInt16.h"
#include "TUnsignedInt16.h"
#include "TSignedInt16.h"

class TInt16 : public TGenericInt16
{
	private:
		bool m_isConst;
		bool m_isSigned;
		bool m_multipleSignedQualifiers;
		std::string m_name;
		TGenericInt16* m_underlyingType;

	public:
		virtual std::string getName() const;
		virtual std::string getInternalName() const;

		virtual bool isConst() const;
		virtual void setConst();
		virtual bool isSigned() const;
		virtual bool setSigned(bool isSigned);

		TInt16(std::string name) : m_isConst(false), m_isSigned(true),
			m_multipleSignedQualifiers(false), m_name(name),
			m_underlyingType(new TSignedInt16()) {}

		// inherits most 16 bit generic integer operations
		// only implements signed or unsigned versions of the non generic operations


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

