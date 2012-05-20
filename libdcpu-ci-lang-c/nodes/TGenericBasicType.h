/**

	File:		TGenericBasicType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TGenericBasicType Type class.

**/

#ifndef __DCPU_COMP_TYPES_GENERICBASICTYPE_H
#define __DCPU_COMP_TYPES_GENERICBASICTYPE_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>

class TGenericBasicType: public IType
{
	private:
		

	public:
		
		virtual std::string getName() const 
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to get name of generic basic type (internal error).");
		}
		
		virtual std::string getInternalName() const 
		{
			throw new CompilerException(0, "<internal>", 
			"Unable to get internal name of generic basic type (internal error).");
		}
		
		bool isBasicType() const;
		bool isStruct() const;
		
		static IType* promoteTypes(IType* typeA, IType* typeB);
};
#endif

