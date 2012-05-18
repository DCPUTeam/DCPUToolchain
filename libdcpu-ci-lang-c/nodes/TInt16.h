#ifndef __DCPU_COMP_TYPES_INT16_H
#define __DCPU_COMP_TYPES_INT16_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <string>
#include <nodes/IType.h>
#include "TGenericInt16.h"

class TInt16 : public TGenericInt16
{
	private:
		

	public:
		virtual std::string getName() const;
	
		// inherits most 16 bit generic integer operations
		// only implements signed versions of the non generic operations
			
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

