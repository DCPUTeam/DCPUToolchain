/**

	File:		TInt16.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TInt Type class.

**/

#include "TInt16.h"

std::string TInt16::getName() const
{
	// TODO maybe call it by its real name? (e.g. int, char, int16_t)
	return "int16_t";
}

std::string TInt16::getInternalName() const
{
	return "int16_t";
}



bool TInt16::implicitCastable(AsmGenerator& context, const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to == "int16_t")
	{
		return true;
	}
	else if (to == "uint16_t")
	{
		return true;
	}
	// FIXME: check for pointer to void here
	else if (to == "ptr16_t")
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool TInt16::explicitCastable(AsmGenerator& context, const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to == "int16_t")
	{
		return true;
	}
	else if (to == "uint16_t")
	{
		return true;
	}
	else if (to == "ptr16_t")
	{
		return true;
	}
	else
	{
		return false;
	}
}

AsmBlock* TInt16::implicitCast(AsmGenerator& context, const IType* toType, char a)
{
	if (!this->implicitCastable(context, toType))
	{
		throw new CompilerException(0, "<internal>", 
		"Unable to implicitly cast integer (internal error).");
	}
	// dont do anything (value stays the same)
	AsmBlock* block = new AsmBlock();
	return block;
}

AsmBlock* TInt16::explicitCast(AsmGenerator& context, const IType* toType, char a)
{
	if (!this->explicitCastable(context, toType))
	{
		throw new CompilerException(0, "<internal>", 
		"Unable to implicitly cast integer (internal error).");
	}
	// TODO if different sizes integers are introduced they have
	//       to be cast here
	AsmBlock* block = new AsmBlock();
	return block;
}


	/* binary operators */


AsmBlock* TInt16::mul(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MLI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TInt16::div(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	DVI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TInt16::mod(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MDI " << a << ", " << b << std::endl;
	return block;
}


		
	/* comparison operators */

// FIXME context dependent stack clear with 0	

AsmBlock* TInt16::gt(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TInt16::lt(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TInt16::ge(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}

AsmBlock* TInt16::le(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}


