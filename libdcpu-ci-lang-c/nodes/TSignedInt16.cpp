/**

	File:		TSignedInt16.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TSignedInt Type class.

**/

#include "TSignedInt16.h"

std::string TSignedInt16::getName() const
{
	return "int16_t";
}

std::string TSignedInt16::getInternalName() const
{
	return "int16_t";
}



bool TSignedInt16::implicitCastable(AsmGenerator& context, const IType* toType)
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

bool TSignedInt16::explicitCastable(AsmGenerator& context, const IType* toType)
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

AsmBlock* TSignedInt16::implicitCast(AsmGenerator& context, const IType* toType, char a)
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

AsmBlock* TSignedInt16::explicitCast(AsmGenerator& context, const IType* toType, char a)
{
	if (!this->explicitCastable(context, toType))
	{
		throw new CompilerException(0, "<internal>",
					    "Unable to implicitly cast integer (internal error).");
	}
	// TODO if different sizes integers are introduced they have
	//	 to be cast here
	AsmBlock* block = new AsmBlock();
	return block;
}


/* binary operators */


AsmBlock* TSignedInt16::mul(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MLI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TSignedInt16::div(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	DVI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TSignedInt16::mod(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MDI " << a << ", " << b << std::endl;
	return block;
}



/* comparison operators */

AsmBlock* TSignedInt16::gt(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TSignedInt16::lt(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TSignedInt16::ge(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}

AsmBlock* TSignedInt16::le(AsmGenerator& context, char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}


