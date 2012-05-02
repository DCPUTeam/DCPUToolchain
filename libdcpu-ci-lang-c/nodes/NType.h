/**

	File:		NType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the NType AST class.

**/

#ifndef __DCPU_COMP_NODES_TYPE_H
#define __DCPU_COMP_NODES_TYPE_H

class NStructureDeclaration;

#include <stdint.h>
#include <nodes/IType.h>
#include "NIdentifier.h"

class NType : public NIdentifier, public IType
{
	private:
		void resolveStruct(AsmGenerator& context);

	protected:
		NType(std::string classifier, const std::string& name, unsigned int pointerCount, bool isStruct) :
			pointerCount(pointerCount), isStruct(isStruct), resolvedStruct(NULL), NIdentifier(name, "type-" + classifier) { }

	public:
		unsigned int pointerCount;
		bool isStruct;
		NStructureDeclaration* resolvedStruct;
		NType(const std::string& name, unsigned int pointerCount, bool isStruct) :
			pointerCount(pointerCount), isStruct(isStruct), resolvedStruct(NULL), NIdentifier(name, "type") { }
		uint16_t getStructFieldPosition(AsmGenerator& context, std::string name);
		size_t getBitSize(AsmGenerator& context);
		virtual uint16_t getWordSize(AsmGenerator& context);
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
		virtual IType& getExpressionType(AsmGenerator& context);

		// Operator overloads.
		bool operator==(const NType& other) const;
		bool operator!=(const NType& other) const;

		// The type that represents types.
		static NType& TypeType;

		// The "void" type.
		static NType& VoidType;
};

#endif
