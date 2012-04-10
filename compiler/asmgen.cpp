/**

	File:			asmgen.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Implemented general classes that assist
					AST nodes to generate assembly.

**/

#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <string>
#include "asmgen.h"
#include "node.h"

// Sets up the assembly generator.
AsmGenerator::AsmGenerator(std::string target)
	: m_CurrentFrame(NULL), m_RootNode(NULL)
{
	// Get a references to the assembler.
	Assembler::loadAll();
	this->m_AssemblerTarget = Assembler::getAssembler(target);

	// Load bootstrap data.
	std::ifstream bootstrap("bootstrap.asm");
	this->m_Preassembly << bootstrap;
	bootstrap.close();
}

// Return the specified function or NULL if none is defined.
NFunctionDeclaration* AsmGenerator::getFunction(std::string name)
{
	for (DeclarationList::iterator i = this->m_RootNode->definitions.begin(); i != this->m_RootNode->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-function")
			if (((NFunctionDeclaration*)*i)->id.name == name)
				return (NFunctionDeclaration*)*i;
	}
	return NULL;
}

// Generates the stack frame for the specified function.
StackFrame* AsmGenerator::generateStackFrame(NFunctionDeclaration* function, bool referenceOnly)
{
	StackFrame::StackMap map;

	// Add stack frame data for arguments.
	for (VariableList::iterator i = function->arguments.begin(); i != function->arguments.end(); i++)		
	{
		map.insert(StackFrame::StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//        other argument declarations (hint: check the map to
		//        see if it already has a match).
	}

	// Add stack frame data for variable declarations.
	for (StatementList::iterator i = function->block.statements.begin(); i != function->block.statements.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			map.insert(StackFrame::StackPair(nvd->id.name, nvd->type));
			// FIXME: Check to make sure variables do not clash with arguments
			//        or other variable declarations (hint: check the map to
			//        see if it already has a match).
		}
	}

	if (!referenceOnly)
	{
		this->m_CurrentFrame = new StackFrame(*this, map);
		return this->m_CurrentFrame;
	}
	else
		return new StackFrame(*this, map);
}

// Frees the stack frame.
void AsmGenerator::finishStackFrame(StackFrame* frame)
{
	if (this->m_CurrentFrame == frame)
	{
		delete this->m_CurrentFrame;
		this->m_CurrentFrame = NULL;
	}
	else
		delete frame;
}

// Calculates the bit size for the specified type, 
size_t AsmGenerator::getTypeBitSize(const NType& type)
{
	if (type.pointerCount > 0)			return 16;
	else if (type.name == "char")		return 16; // 8
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

// Calculates the byte size for the specified type, 
size_t AsmGenerator::getTypeWordSize(const NType& type)
{
	return (int)std::ceil((double)this->getTypeBitSize(type) / 16.0);
}

// Generates a random, unique label for use in code.
std::string AsmGenerator::getRandomLabel(std::string prefix)
{
	std::string result = "";
	while ((result == "") || (std::find(this->m_AutomaticLabels.begin(), this->m_AutomaticLabels.end(), result) != this->m_AutomaticLabels.end()))
		result = "__" + prefix + "_" + AsmGenerator::getRandomString(10);
	return result;
}

// Generates a random character.
char AsmGenerator::getRandomCharacter()
{
    unsigned char c;
    while (!std::isalnum(c = static_cast<unsigned char>(std::rand() % 256))) ;
    return c;
}

// Generates a random string.
std::string AsmGenerator::getRandomString(std::string::size_type sz)
{
    std::string s;
    s.reserve(sz);
    std::generate_n(std::back_inserter(s), sz, AsmGenerator::getRandomCharacter);
    return s;
}

// Gets a relative stack position by the variable name.
int32_t StackFrame::getPositionOfVariable(std::string name)
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)
	{
		if ((*i).first == name)
			return size;
		else
			size += this->m_Generator.getTypeWordSize((*i).second);
	}
	return -1;
}

// Gets the total size of the stack frame.
uint16_t StackFrame::getSize()
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)		
		size += this->m_Generator.getTypeWordSize((*i).second);
	return size;
}

//
// Assembly block functions.
//

AsmBlock::AsmBlock()
{
}

AsmBlock& operator<< (AsmBlock& block, char& input)
{
	std::stringstream sstr;
	sstr << input;
	return block << sstr.str();
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