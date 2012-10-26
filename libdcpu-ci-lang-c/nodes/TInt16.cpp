/**

    File:       TInt16.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the TInt Type class.

**/

#include "TInt16.h"

std::string TInt16::getName() const
{
    return this->m_name;
}

std::string TInt16::getInternalName() const
{
    return "int16_t";
}

bool TInt16::isConst() const
{
    return this->m_isConst;
}

void TInt16::setConst()
{
    this->m_isConst = true;
}

bool TInt16::isSigned() const
{
    return this->m_isSigned;
}

bool TInt16::setSigned(bool isSigned)
{
    if (!this->m_multipleSignedQualifiers)
    {
        if (m_isSigned != isSigned)
        {
            this->m_isSigned = isSigned;
            if (!this->m_isSigned)
            {
                delete this->m_underlyingType;
                this->m_underlyingType = new TUnsignedInt16();
            }
        }

        this->m_multipleSignedQualifiers = true;
        return true;
    }
    else
    {
        return false;
    }
}


bool TInt16::implicitCastable(AsmGenerator& context, const IType* toType)
{
    return this->m_underlyingType->implicitCastable(context, toType);
}
bool TInt16::explicitCastable(AsmGenerator& context, const IType* toType)
{
    return this->m_underlyingType->explicitCastable(context, toType);
}

AsmBlock* TInt16::implicitCast(AsmGenerator& context, const IType* toType, char a)
{
    return this->m_underlyingType->implicitCast(context, toType, a);
}

AsmBlock* TInt16::explicitCast(AsmGenerator& context, const IType* toType, char a)
{
    return this->m_underlyingType->explicitCast(context, toType, a);
}


/* binary operators */


AsmBlock* TInt16::mul(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->mul(context, a, b);
}

AsmBlock* TInt16::div(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->div(context, a, b);
}

AsmBlock* TInt16::mod(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->mod(context, a, b);
}



/* comparison operators */

AsmBlock* TInt16::gt(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->gt(context, a, b);
}

AsmBlock* TInt16::lt(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->lt(context, a, b);
}

AsmBlock* TInt16::ge(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->ge(context, a, b);
}

AsmBlock* TInt16::le(AsmGenerator& context, char a, char b)
{
    return this->m_underlyingType->le(context, a, b);
}


