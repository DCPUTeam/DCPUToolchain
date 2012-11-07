/**

    File:           TypePosition.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the TypePosition class.

**/

#ifndef __DCPU_LIBDCC_TYPEPOSITION_H
#define __DCPU_LIBDCC_TYPEPOSITION_H

#include <stdint.h>
#include <string>

class TypePosition
{
private:
    bool m_Found;
    bool m_Global;
    bool m_Function;
    bool m_IsFunctionParameter;
    bool m_PreviousStackFrame;
    uint16_t m_Position;
    std::string m_FunctionName;
public:
    TypePosition(bool isFound, bool isGlobal, bool isFunctionParameter, bool previousStackFrame, uint16_t position);
    TypePosition(bool isFound, std::string funcName);
    TypePosition() : m_Found(false), m_Global(false), m_Function(false), m_IsFunctionParameter(false), m_Position(0) {}
    bool isFound();
    bool isFunction();
    bool isAtomiclyAddressable();
    std::string getAtomicAddress();
    std::string pushAddress(char registr);
};

#endif
