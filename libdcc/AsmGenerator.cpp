/**

	File:			AsmGenerator.cpp

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Defines the AsmGenerator class.

**/

#include <fstream>
#include <cctype>
#include <algorithm>
#include <iterator>
#include "AsmGenerator.h"

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
IFunctionDeclaration* AsmGenerator::getFunction(std::string name)
{
	return this->m_RootNode->getFunction(name);
}

// Generates the stack frame for the specified function.
StackFrame* AsmGenerator::generateStackFrame(IFunctionDeclaration* function, bool referenceOnly)
{
	StackMap map = function->generateStackMap();

	if (!referenceOnly)
	{
		this->m_CurrentFrame = new StackFrame(*this, map);
		return this->m_CurrentFrame;
	}
	else
		return new StackFrame(*this, map);
}

// Generates an incomplete stack frame for the specified function.
StackFrame* AsmGenerator::generateStackFrameIncomplete(IFunctionSignature* signature)
{
	StackMap map = signature->generateStackMap();

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