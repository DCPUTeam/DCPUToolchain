/**

	File:		TGenericBasicType.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TGenericBasicType Type class.

**/

#include "TGenericBasicType.h"

bool TGenericBasicType::isBasicType() const
{
	return true;
}

bool TGenericBasicType::isStruct() const
{
	return false;
}

IType* TGenericBasicType::promoteTypes(IType* typeA, IType* typeB)
{
	std::string A = typeA->getInternalName();
	std::string B = typeB->getInternalName();
	if (A == B) {
		return typeA;
	/* Casting according to ANSI C $3.2.1.5 	*/
	/* Searching in order:		    (in DCPU?)	*/
	/*    - long double			o	*/
	/*    - double				o	*/
	/*    - float				o	*/
	/*    - long long unsigned int		o	*/
	/*    - long long int			o	*/
	/*    - long unsigned int		o	*/
	/*    - long int			o	*/
	/*    - unsigned int			x	*/
	/*    - int				x	*/
	
	} else if (A == "uint16_t" || B == "uint16_t") {
		if (A == "uint16_t")
			return typeA;
		else
			return typeB;
	} else if (A == "int16_t" || B == "int16_t") {
		if (A == "int16_t")
			return typeA;
		else
			return typeB;
	} else {
		throw new CompilerException(0, "<internal>", 
		"Unable to promote unknown basic type (internal error)."); 
	}
}

