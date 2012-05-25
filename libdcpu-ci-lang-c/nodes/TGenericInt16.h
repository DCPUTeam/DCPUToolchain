/**

	File:		TGenericInt16.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TGenericInt Type class.

**/

#ifndef __DCPU_COMP_TYPES_GENERICINT16_H
#define __DCPU_COMP_TYPES_GENERICINT16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>
#include "TGenericBasicType.h"

class TGenericInt16: public TGenericBasicType
{
	private:


	public:
		virtual uint16_t getWordSize(AsmGenerator& context);

		virtual std::string getName()
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to get name of generic integer (internal error).");
		}

		/* copy */
		// direct copy via registers
		virtual AsmBlock* copyValue(AsmGenerator& context, char from, char to);
		// indirect copy given references (copies values)
		virtual AsmBlock* copyByRef(AsmGenerator& context, char fromRef, char toRef);
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(AsmGenerator& context, char from, char toRef);
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(AsmGenerator& context, char fromRef, char to);


		/* stack ops */
		virtual AsmBlock* pushStack(AsmGenerator& context, char a);
		// FIXME do i need this?
		// virtual AsmBlock* pushStackByRef(char a);
		virtual AsmBlock* popStack(AsmGenerator& context);
		virtual AsmBlock* popStackReturn(AsmGenerator& context, char a);
		// for debugging, cleaning the stack while pop
		virtual AsmBlock* popStackClean(AsmGenerator& context);
		virtual AsmBlock* popStackCleanReturn(AsmGenerator& context, char a);

		/* binary operators */
		virtual AsmBlock* add(AsmGenerator& context, char a, char b);
		virtual AsmBlock* sub(AsmGenerator& context, char a, char b);
		virtual AsmBlock* band(AsmGenerator& context, char a, char b);
		virtual AsmBlock* bor(AsmGenerator& context, char a, char b);
		virtual AsmBlock* bxor(AsmGenerator& context, char a, char b);
		virtual AsmBlock* shl(AsmGenerator& context, char a, char b);
		virtual AsmBlock* shr(AsmGenerator& context, char a, char b);
		virtual AsmBlock* land(AsmGenerator& context, char a, char b);
		virtual AsmBlock* lor(AsmGenerator& context, char a, char b);

		// non generic binary operations:
		virtual AsmBlock* mul(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to multiply generic integer (internal error).");
		}
		virtual AsmBlock* div(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to divide generic integer (internal error).");
		}
		virtual AsmBlock* mod(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to modulo generic integer (internal error).");
		}


		/* unary operators */
		virtual AsmBlock* plus(AsmGenerator& context, char a);
		virtual AsmBlock* minus(AsmGenerator& context, char a);
		virtual AsmBlock* bnot(AsmGenerator& context, char a);
		virtual AsmBlock* lnot(AsmGenerator& context, char a);
		virtual AsmBlock* inc(AsmGenerator& context, char ref);
		virtual AsmBlock* dec(AsmGenerator& context, char ref);

		/* comparison operators */
		virtual AsmBlock* eq(AsmGenerator& context, char a, char b);
		virtual AsmBlock* neq(AsmGenerator& context, char a, char b);

		// non generic comparison operators:
		virtual AsmBlock* gt(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* lt(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* ge(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* le(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Unable to compare generic integer (internal error).");
		}


};
#endif

