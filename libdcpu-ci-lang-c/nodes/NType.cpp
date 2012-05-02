/**

	File:		NType.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NType AST class.

**/

#include <stdexcept>
#include <cmath>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NDeclarations.h"
#include "NStructureDeclaration.h"
#include "Lists.h"

NType internal_TypeType("<type>", 0, false);
NType internal_VoidType("void", 0, false);
NType& NType::TypeType(internal_TypeType);
NType& NType::VoidType(internal_VoidType);

AsmBlock* NType::compile(AsmGenerator& context)
{
	throw new CompilerException("Unable to compile type AST node.");
}

AsmBlock* NType::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a type.");
}

void NType::resolveStruct(AsmGenerator& context)
{
	if (this->resolvedStruct != NULL)
		return;

	// Search AST for struct nodes.
	NDeclarations* decls = (NDeclarations*)context.m_RootNode;

	for (DeclarationList::iterator i = decls->definitions.begin(); i != decls->definitions.end(); i++)
		if ((*i)->cType == "statement-declaration-structure")
			if (((NStructureDeclaration*)*i)->id.name == this->name)
			{
				this->resolvedStruct = ((NStructureDeclaration*)*i);
				return;
			}

	throw new CompilerException("Unknown struct type " + this->name + " encountered!");
}

size_t NType::getBitSize(AsmGenerator& context)
{
	if (this->pointerCount > 0)
		return 16;
	else if (this->isStruct)
	{
		this->resolveStruct(context);
		return this->resolvedStruct->getSize(context);
	}
	else
	{
		if (this->name == "char")			return 16; // 8
		else if (this->name == "byte")		return 16; // 8
		else if (this->name == "short")		return 16;
		else if (this->name == "int")		return 32;
		else if (this->name == "long")		return 64;
		else if (this->name == "int8_t")	return 16; // 8
		else if (this->name == "int16_t")	return 16;
		else if (this->name == "int32_t")	return 32;
		else if (this->name == "int64_t")	return 64;
		else if (this->name == "uint8_t")	return 16; // 8
		else if (this->name == "uint16_t")	return 16;
		else if (this->name == "uint32_t")	return 32;
		else if (this->name == "uint64_t")	return 64;
		else
			throw new CompilerException("Unknown type " + this->name + " encountered!");
	}
}

uint16_t NType::getWordSize(AsmGenerator& context)
{
	return (int)std::ceil((double)this->getBitSize(context) / 16.0);
}

uint16_t NType::getStructFieldPosition(AsmGenerator& context, std::string name)
{
	// Resolve struct if not already done.
	this->resolveStruct(context);

	// Count up the position.
	size_t pos = 0;

	for (VariableList::iterator i = this->resolvedStruct->fields.begin(); i != this->resolvedStruct->fields.end(); i++)
	{
		if ((*i)->id.name == name)
			return pos;
		else
			pos += (*i)->type.getWordSize(context);
	}

	// If the field wasn't found...
	throw new CompilerException("Unable to lookup field " + name + " in structure " + this->resolvedStruct->id.name + "!");
}

IType& NType::getExpressionType(AsmGenerator& context)
{
	// Types are of type type ;)
	return NType::TypeType;
}

bool NType::operator==(const NType& other) const
{
	return (this->name == other.name &&
		this->isStruct == other.isStruct &&
		this->pointerCount == other.pointerCount);
}

bool NType::operator!=(const NType& other) const
{
	return !(*this == other);
}
