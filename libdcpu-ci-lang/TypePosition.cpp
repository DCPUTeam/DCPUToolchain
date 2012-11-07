/**

    File:           TypePosition.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the TypePosition class.

**/

#include <sstream>
#include "TypePosition.h"
#include "CompilerException.h"

TypePosition::TypePosition(bool isFound, bool isGlobal, bool isFunctionParameter, bool previousStackFrame, uint16_t position)
{
    this->m_Found = isFound;
    this->m_Function = false;
    this->m_FunctionName = "";
    this->m_Global = isGlobal;
    this->m_IsFunctionParameter = isFunctionParameter;
    this->m_PreviousStackFrame = previousStackFrame;
    this->m_Position = position;
}

TypePosition::TypePosition(bool isFound, std::string funcName)
{
    this->m_Found = true;
    this->m_Function = true;
    this->m_FunctionName = funcName;
    this->m_Global = true;
    this->m_IsFunctionParameter = false;
    this->m_PreviousStackFrame = false;
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

bool TypePosition::isAtomiclyAddressable()
{
    if (!this->m_Found)
        return false;
        
    if (this->m_PreviousStackFrame)
        return false;
        
    if (this->m_Global && (!this->m_Function))
        return false;
        
    return true;
}


std::string TypePosition::getAtomicAddress()
{
    std::stringstream sstr;

    if (!this->m_Found)
        throw new CompilerException(0, "<internal>", "Attempted to get reference position of unknown type position result (internal error).");

    if (this->m_PreviousStackFrame)
        throw new CompilerException(0, "<internal>", "Attempted to get reference position of previous Stack Frame");

    if (this->m_Function)
        sstr << "cfunc_" << this->m_FunctionName;
    else if (this->m_Global)
        // FIXME: once expressions are possible in the linker, we can do this!
        throw new CompilerException(0, "<internal>", "Attempted to get reference position of global variable (will only be possible once expressions like '_DATA+1' are implemented in the linker)");
        //sstr << "_DATA+" << this->m_Position;
    else if (this->m_IsFunctionParameter)
        sstr << "Y+" << this->m_Position;
    else
        sstr << "Y+" << (uint16_t)(0x0 - (this->m_Position + 2));

    return sstr.str();
}


std::string TypePosition::pushAddress(char registr)
{
    std::stringstream sstr;

    TypePosition * blah = NULL;
    

    if (!this->m_Found)
    {
        blah->pushAddress('A');
        throw new CompilerException(0, "<internal>", "Attempted to push reference position of unknown type position result (internal error).");
    }

    if (this->m_Function)
        sstr << "   SET " << registr << ", cfunc_" << this->m_FunctionName << std::endl;
    else if (this->m_Global)
    {
        sstr << "   SET " << registr << ", _DATA" << std::endl;
        if (this->m_Position != 0)
            // we use the positions from here on _downwards_
            sstr << "   ADD " << registr << ", " << this->m_Position << std::endl;
    }
    else
    {
        // two more cases:
        //  local variables: is in the Stack in Position Y-3-(relative local position)
        //  function parameter: in Stack in Position Y+(relative local position)

        // get current stack frame address
        sstr << "   SET " << registr << ", Y" << std::endl;

        // do we want to reference the previous stack frame?
        if (this->m_PreviousStackFrame)
        {
            sstr << "   SUB " << registr << ", 1" << std::endl;
            sstr << "   SET " << registr << ", [" << registr << "]" << std::endl;
        }


        if (this->m_IsFunctionParameter)
        {
            // function parameters in stack position Y+pos
            if (this->m_Position != 0)
                sstr << "   ADD " << registr << ", " << this->m_Position << std::endl;
        }
        else
        {
            // last word of first local is at Y-3, but position of first local
            // includes its word size, thus Y-2-pos = Y - (2+pos)
            sstr << "   SUB " << registr << ", " << (uint16_t) (2+this->m_Position) << std::endl;
        }
    }

    return sstr.str();
}
