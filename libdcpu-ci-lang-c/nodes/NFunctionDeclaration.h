/**

	File:           NFunctionDeclaration.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes

	Description:    Declares the NFunctionDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_DECLARATION_H
#define __DCPU_COMP_NODES_FUNCTION_DECLARATION_H

#include <nodes/IFunctionDeclaration.h>
#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NBlock.h"
#include "NType.h"
#include "NFunctionPointerType.h"

class NFunctionDeclaration : public NDeclaration, public NFunctionSignature, public IFunctionDeclaration
{
public:
	const NIdentifier& id;
	NBlock* block;
	NFunctionPointerType* pointerType;
	NFunctionDeclaration(const NType& type, const NIdentifier& id, const VariableList& arguments, NBlock* block);
	~NFunctionDeclaration();
	virtual AsmBlock* compile(AsmGenerator& context);
	virtual AsmBlock* reference(AsmGenerator& context);
	virtual StackMap generateStackMap();
	virtual IType* getPointerType();
};

#endif
