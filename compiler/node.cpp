//
// DCPU Compiler by James Rhodes
//
// AST node definitions.
//

#include <iomanip>
#include "node.h"

AsmBlock* NDeclarations::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// First deal with the main setup.
	NFunctionDeclaration* main = context.getMainFunction(this);
	if (main == NULL)
	{
		delete block;
		return NULL;
	}

	// Get the stack size required of main.
	size_t stack_size = context.getStackSize(main);

	// Output assembly for calling main.
	*block <<  "	SET X, " << stack_size << std::endl;
	*block <<  "	SET Z, _halt" << std::endl;
	*block <<  "	SET I, main" << std::endl;
	*block <<  "	SET PC, _stack_call" << std::endl;

	// Handle function definitions.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		AsmBlock* inner = (*i)->compile(context);
		*block << *inner;
		if (inner != NULL)
			delete inner;
	}

	return block;
}

AsmBlock* NFunctionDeclaration::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Calculate total stack space required.
	size_t stack_size = context.getStackSize(this);

	// Output the label.
	*block <<  ":" << this->id.name << std::endl;

	// Debug testing; write the name of the function to the screen.
	unsigned int ma = 0x8000;
	std::string mn = this->id.name;
	for (int i = 0; i < mn.length(); i += 1)
	{
		int chr = mn.at(i);
		std::stringstream sstr;
		sstr << std::hex << "SET [0x" << ma << "], 0x70" << chr;
		*block <<   "	" << sstr.str() << std::endl;
		ma += 1;
	}
	*block <<  "	SET PC, _halt" << std::endl;

	return block;
}

AsmBlock* NInteger::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NCharacter::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NString::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NIdentifier::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NType::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NBlock::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NVariableDeclaration::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NExpressionStatement::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NAssignment::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NBinaryOperator::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NMethodCall::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NIfStatement::compile(AsmGenerator& context)
{
	return NULL;
}

AsmBlock* NReturnStatement::compile(AsmGenerator& context)
{
	return NULL;
}