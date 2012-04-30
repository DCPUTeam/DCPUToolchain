/**

	File:           Assembler.cpp

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang

	Authors:        James Rhodes

	Description:    Defines what feature support different
	                assemblers have.

**/

#include <exception>
#include <stdexcept>
#include "Assembler.h"

// We haven't initially loaded anything.
bool Assembler::m_HasLoaded = false;
std::map<std::string, const Assembler*> Assembler::m_Assemblers;

// Loads all of the assembler definitions, if they
// are not already loaded.
void Assembler::loadAll()
{
	if (m_HasLoaded) return; else m_HasLoaded = true;

	// Define assemblers.
	m_Assemblers.insert(entry("dcpu16toolchain", new Assembler(true, false, true, true, true, true, true)));
	m_Assemblers.insert(entry("dcpu16studio", new Assembler(false, false, true, false, false, false, false)));
}

// Gets a reference to an assembler by it's name.
const Assembler* Assembler::getAssembler(std::string name)
{
	std::map<std::string, const Assembler*>::iterator i = Assembler::m_Assemblers.find(name);
	if (i == Assembler::m_Assemblers.end())
		throw new std::runtime_error("Requested assembler information not found (internal error).");
	else
		return i->second;
}

// Returns whether the specified assembler exists.
bool Assembler::hasAssembler(std::string name)
{
	std::map<std::string, const Assembler*>::iterator i = Assembler::m_Assemblers.find(name);
	return (i != Assembler::m_Assemblers.end());
}
