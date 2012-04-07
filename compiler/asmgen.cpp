//
// DCPU Compiler by James Rhodes
//
// Assembly generation.
//

#include <sstream>
#include <fstream>
#include "asmgen.h"
#include "node.h"

// Sets up the assembly generator.
AsmGenerator::AsmGenerator()
{
	std::ifstream bootstrap("bootstrap.asm");
	this->m_Preassembly << bootstrap;
	bootstrap.close();
}

// Return the main function or NULL if none is defined.
NFunctionDeclaration* AsmGenerator::getMainFunction(NDeclarations* decls)
{
	for (DeclarationList::iterator i = decls->definitions.begin(); i != decls->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-function")
			if (((NFunctionDeclaration*)*i)->id.name == "main")
				return (NFunctionDeclaration*)*i;
	}
	return NULL;
}

// Calculates the total required stack space (minus the return
// address) for the specified function.
size_t AsmGenerator::getStackSize(NFunctionDeclaration* function)
{
	size_t size = 0;
	/*for (VariableList::iterator i = function->arguments.begin(); i != function->arguments.end(); i++)
		size += this->getTypeSize(*i
	if (function->arguments*/
	return 8;
}

//
// Assembly block functions.
//

AsmBlock::AsmBlock()
{
}

AsmBlock& operator<< (AsmBlock& block, const size_t& input)
{
	std::stringstream sstr;
	sstr << input;
	return block << sstr.str();
}

AsmBlock& operator<< (AsmBlock& block, const char input[])
{
	return block << std::string(input);
}

AsmBlock& operator<< (AsmBlock& block, std::ifstream& input)
{
	std::string data((std::istreambuf_iterator<char>(input)),
					  std::istreambuf_iterator<char>());
	block.m_Assembly += data;
	return block;
}

AsmBlock& operator<< (AsmBlock& block, const std::string& input)
{
	block.m_Assembly += input;
	return block;
}

AsmBlock& operator<< (AsmBlock& block, const AsmBlock& input)
{
	if (&input == NULL) return block;
	block.m_Assembly += input.m_Assembly + "\n";
	return block;
}

std::ostream& operator<< (std::ostream& output, const AsmBlock& block)
{
	output << block.m_Assembly;
	return output;
}