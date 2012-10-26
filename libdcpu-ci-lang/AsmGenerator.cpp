/**

    File:       AsmGenerator.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang

    Authors:    James Rhodes

    Description:    Defines the AsmGenerator class.

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

    this->m_loopStack = std::deque<std::pair<std::string, std::string> >();
}

// Return the specified function or NULL if none is defined.
IFunctionDeclaration* AsmGenerator::getFunction(std::string name)
{
    return this->m_RootNode->getFunction(name);
}

// Generates the stack frame for the specified function.
StackFrame* AsmGenerator::generateStackFrame(IFunctionDeclaration* function, bool referenceOnly)
{
    StackMap parameters = function->generateParametersStackMap();

    if (!referenceOnly)
    {
        StackMap locals = function->generateLocalsStackMap();
        this->m_CurrentFrame = new StackFrame(*this, parameters, locals);
        return this->m_CurrentFrame;
    }
    else
        return new StackFrame(*this, parameters);
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

void AsmGenerator::initLoopStack()
{
    this->m_loopStack.clear();
}

void AsmGenerator::popLoopStack()
{
    this->m_loopStack.pop_back();
}

void AsmGenerator::pushLoopStack(std::string breakLabel, std::string continueLabel)
{
    this->m_loopStack.push_back(std::pair<std::string, std::string>(breakLabel, continueLabel));
}

std::string AsmGenerator::getContinueLabel()
{
    std::string result = "";
    for (std::deque<std::pair<std::string, std::string> >::reverse_iterator it = this->m_loopStack.rbegin(); it != this->m_loopStack.rend(); it++)
    {
        if (it->second != result)
        {
            result = it->second;
            return result;
        }
    }
    return result;
}

std::string AsmGenerator::getBreakLabel()
{
    if (this->m_loopStack.size() > 0)
    {
        return this->m_loopStack.back().first;
    }
    else
    {
        return std::string("");
    }
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
