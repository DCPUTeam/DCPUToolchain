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
		virtual AsmBlock* copyValue(char from, char to);
		// indirect copy given references (copies values)
		virtual AsmBlock* copyByRef(char fromRef, char toRef);
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(char from, char toRef);
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(char fromRef, char to);
		
		
		/* stack ops */
		virtual AsmBlock* pushStack(char a);
		// FIXME do i need this? 
		// virtual AsmBlock* pushStackByRef(char a);
		virtual AsmBlock* popStack();
		virtual AsmBlock* popStackReturn(char a);
		// for debugging, cleaning the stack while pop
		virtual AsmBlock* popStackClean();
		virtual AsmBlock* popStackCleanReturn(char a);
		
		/* binary operators */
		virtual AsmBlock* add(char a, char b);
		virtual AsmBlock* sub(char a, char b);
		virtual AsmBlock* band(char a, char b);
		virtual AsmBlock* bor(char a, char b);
		virtual AsmBlock* bxor(char a, char b);
		virtual AsmBlock* shl(char a, char b);
		virtual AsmBlock* shr(char a, char b);
		virtual AsmBlock* land(char a, char b);
		virtual AsmBlock* lor(char a, char b);
				
		// non generic binary operations:
		virtual AsmBlock* mul(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to multiply generic integer (internal error).");
		}
		virtual AsmBlock* div(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to divide generic integer (internal error).");
		}
		virtual AsmBlock* mod(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to modulo generic integer (internal error).");
		}
		
		
		/* unary operators */
		virtual AsmBlock* plus(char a);
		virtual AsmBlock* minus(char a);
		virtual AsmBlock* bnot(char a);
		virtual AsmBlock* lnot(char a);
		virtual AsmBlock* inc(char a);
		virtual AsmBlock* dec(char a);
		
		/* comparison operators */
		virtual AsmBlock* eq(char a, char b);
		virtual AsmBlock* neq(char a, char b);
		
		// non generic comparison operators:
		virtual AsmBlock* gt(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* lt(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* ge(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to compare generic integer (internal error).");
		}
		virtual AsmBlock* le(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to compare generic integer (internal error).");
		}
		
		
};
#endif

