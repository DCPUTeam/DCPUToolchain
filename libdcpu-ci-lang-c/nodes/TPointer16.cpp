
#include "TPointer16.h"

std::string TPointer16::getName() const
{
	return "(" + m_pointingTo->getName() + ")*";
}


IType* TPointer16::getPointerBaseType()
{
	return this->m_pointingTo;
}
