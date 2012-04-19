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

	// Set the global frame to NULL as it doesn't
	// exist until the NDeclarations root node is processed.
	this->m_GlobalFrame = NULL;

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

	// This function can not be called on functions with no body (because
	// that means they were an import and hence have no information
	// about the variables).
	if (function->block == NULL)
		throw new CompilerException("Can not generate a stack frame for a function declaration with no body.");

	// Add stack frame data for arguments.
	for (VariableList::const_iterator i = function->arguments.begin(); i != function->arguments.end(); i++)		
	{
		map.insert(map.end(), StackFrame::StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//        other argument declarations (hint: check the map to
		//        see if it already has a match).
	}

	// Add stack frame data for variable declarations.
	for (StatementList::iterator i = function->block->statements.begin(); i != function->block->statements.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			map.insert(map.end(), StackFrame::StackPair(nvd->id.name, nvd->type));
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

// Generates an incomplete stack frame for the specified function.
StackFrame* AsmGenerator::generateStackFrameIncomplete(NFunctionSignature* signature)
{
	StackFrame::StackMap map;

	// Add stack frame data for arguments.
	for (VariableList::const_iterator i = signature->arguments.begin(); i != signature->arguments.end(); i++)		
	{
		map.insert(map.end(), StackFrame::StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//        other argument declarations (hint: check the map to
		//        see if it already has a match).
	}

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
TypePosition StackFrame::getPositionOfVariable(std::string name, bool stackStartAtC)
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)
	{
		if ((*i).first == name)
			return TypePosition(true, this->m_Generator.m_GlobalFrame == this, stackStartAtC, size);
		else
			size += (*i).second.getWordSize(this->m_Generator);
	}
	if (this->m_Generator.m_GlobalFrame == NULL || this->m_Generator.m_GlobalFrame == this)
	{
		// Also search defined functions.
		NFunctionDeclaration* func = this->m_Generator.getFunction(name);
		if (func != NULL)
			return TypePosition(true, name);
		else
			return TypePosition(false, false, false, 0);
	}
	else
		return this->m_Generator.m_GlobalFrame->getPositionOfVariable(name);
}

// Gets a pointer to the type of a variable based on it's name.
NType* StackFrame::getTypeOfVariable(std::string name)
{
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)
	{
		if ((*i).first == name)
			return &((*i).second);
	}
	if (this->m_Generator.m_GlobalFrame == NULL || this->m_Generator.m_GlobalFrame == this)
	{
		// Also search defined functions.
		NFunctionDeclaration* func = this->m_Generator.getFunction(name);
		if (func != NULL)
			return func->pointerType;
		else
			return NULL;
	}
	else
		return this->m_Generator.m_GlobalFrame->getTypeOfVariable(name);
}

// Gets the total size of the stack frame.
uint16_t StackFrame::getSize()
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)		
		size += (*i).second.getWordSize(this->m_Generator);
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

//
// Type position functions.
//

TypePosition::TypePosition(bool isFound, bool isGlobal, bool hasStackStartAtC, uint16_t position)
{
	this->m_Found = isFound;
	this->m_Function = false;
	this->m_FunctionName = "";
	this->m_Global = isGlobal;
	this->m_StackStartAtC = hasStackStartAtC;
	this->m_Position = position;
}

TypePosition::TypePosition(bool isFound, std::string funcName)
{
	this->m_Found = true;
	this->m_Function = true;
	this->m_FunctionName = funcName;
	this->m_Global = true;
	this->m_StackStartAtC = false;
	this->m_Position = 0;
}

bool TypePosition::isFound()
{
	return this->m_Found;
}

bool TypePosition::isFunction()
{
	return this->m_Function;
}

std::string TypePosition::pushAddress(char registr)
{
	std::stringstream sstr;
	if (!this->m_Found)
		throw new CompilerException("Attempted to push reference position of unknown type position result (internal error).");
	if (this->m_Function)
		sstr << "	SET " << registr << ", cfunc_" << this->m_FunctionName << std::endl;
	else if (this->m_Global)
		sstr << "	SET " << registr << ", _DATA" << std::endl;
	else if (this->m_StackStartAtC)
	{
		sstr << "	SET " << registr << ", C" << std::endl;
		sstr << "	ADD " << registr << ", 1" << std::endl;
	}
	else
		sstr << "	SET " << registr << ", Y" << std::endl;
	if (this->m_Position != 0)
		sstr << "	ADD " << registr << ", " << this->m_Position << std::endl;
	return sstr.str();
}