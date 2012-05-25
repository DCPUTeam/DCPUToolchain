/**

	File:		NArrayDeclaration.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the NArrayDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_ARRAY_DECLARATION_H
#define __DCPU_COMP_NODES_ARRAY_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NExpression.h"
#include "NType.h"
#include "TPointer16.h"
#include "Lists.h"

class NArrayDeclaration : public NDeclaration
{
		DimensionsList* m_dims;
		ExpressionList* m_initExprs;

		uint16_t m_numElements;

		IType* m_baseType;
		IType* m_pointerType;
		IType* m_memAreaType;

	public:

		NIdentifier& id;

		IType* getPointerType();
		IType* getMemAreaType();

		NArrayDeclaration(IType* type, NIdentifier& id, DimensionsList* dims, ExpressionList* initExprList);
		virtual AsmBlock* compile(AsmGenerator& context);
		virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
