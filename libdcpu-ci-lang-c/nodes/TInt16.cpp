
#include "TInt16.h"

std::string TInt16::getName() const
{
	// TODO maybe call it by its real name? (e.g. int, char, int16_t)
	return "int16_t";
}


	/* binary operators */


AsmBlock* TInt16::mul(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MLI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TInt16::div(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	DVI " << a << ", " << b << std::endl;
	return block;
}

AsmBlock* TInt16::mod(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MDI " << a << ", " << b << std::endl;
	return block;
}


		
	/* comparison operators */

// FIXME context dependent stack clear with 0	

AsmBlock* TInt16::gt(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TInt16::lt(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 0" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 1" << std::endl;
	*block <<	"	SET " << a << ", POP" << std::endl;
	return block;
}

AsmBlock* TInt16::ge(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFU " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}

AsmBlock* TInt16::le(char a, char b)
{
	AsmBlock* block = new AsmBlock();
	// stack access is more efficient than using SUB and EX
	*block <<	"	SET PUSH, 1" << std::endl;
	*block <<	"	IFA " << a << ", " << b << std::endl;
	*block <<	"		SET PEEK, 0" << std::endl;
	*block <<	"	SET A, POP" << std::endl;
	return block;
}


