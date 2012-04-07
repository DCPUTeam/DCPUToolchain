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
	for (VariableList::iterator i = function->arguments.begin(); i != function->arguments.end(); i++)		
		size += this->getTypeSize((*i)->type);
	return size;
}

// Calculates the size for the specified type, 
size_t AsmGenerator::getTypeSize(const NType& type)
{
	if (type.pointerCount > 0)			return 16;
	else if (type.name == "byte")		return 16; // 8
	else if (type.name == "short")		return 16;
	else if (type.name == "int")		return 32;
	else if (type.name == "long")		return 64;
	else if (type.name == "int8_t")		return 16; // 8
	else if (type.name == "int16_t")	return 16;
	else if (type.name == "int32_t")	return 32;
	else if (type.name == "int64_t")	return 64;
	else if (type.name == "uint8_t")	return 16; // 8
	else if (type.name == "uint16_t")	return 16;
	else if (type.name == "uint32_t")	return 32;
	else if (type.name == "uint64_t")	return 64;
	else
		throw new CompilerException("Unknown type " + type.name + " encountered!");
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