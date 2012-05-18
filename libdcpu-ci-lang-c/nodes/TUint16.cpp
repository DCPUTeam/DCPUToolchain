
#include "TUint16.h"

std::string TUint16::getName() const
{
	// TODO maybe call it by its real name? (e.g. unsigned int, unsigned char, uint16_t)
	return "uint16_t";
}

std::string TUint16::getInternalName() const
{
	return "uint16_t";
}


bool TUint16::implicitCastable(const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to == "uint16_t")
	{
		return true;
	}
	else if (to == "int16_t")
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
bool TUint16::explicitCastable(const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to == "uint16_t")
	{
		return true;
	}
	else if (to == "int16_t")
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

AsmBlock* TUint16::implicitCast(const IType* toType, char a)
{
	if (!this->implicitCastable(toType))
	{
		throw new CompilerException(0, "<internal>", 
		"Unable to implicitly cast integer (internal error).");
	}
	// dont do anything (value stays the same)
	AsmBlock* block = new AsmBlock();
	return block;
}

AsmBlock* TUint16::explicitCast(const IType* toType, char a)
{
	if (!this->explicitCastable(toType))
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


AsmBlock* TUint16::mul(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MUL " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TUint16::div(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	DIV " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TUint16::mod(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MOD " << a << ", " << b << std::endl;
	return block;
}


		
	/* comparison operators */

// FIXME context dependent stack clear with 0	

AsmBlock* TUint16::gt(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFG " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TUint16::lt(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFL " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TUint16::ge(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFL " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}

AsmBlock* TUint16::le(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFG " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}


