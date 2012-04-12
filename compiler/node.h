/**

	File:			node.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares class hierarchy for AST nodes
					in the compiler.

**/

#ifndef __DCPU_COMP_NODE_H
#define __DCPU_COMP_NODE_H

#include "nodes/Lists.h"
#include "nodes/NAddressOfOperator.h"
#include "nodes/NAssignment.h"
#include "nodes/NBinaryOperator.h"
#include "nodes/NBlock.h"
#include "nodes/NCharacter.h"
#include "nodes/NDebugStatement.h"
#include "nodes/NDeclaration.h"
#include "nodes/NDeclarations.h"
#include "nodes/NDereferenceOperator.h"
#include "nodes/NExpression.h"
#include "nodes/NExpressionStatement.h"
#include "nodes/NForStatement.h"
#include "nodes/NFunctionDeclaration.h"
#include "nodes/NFunctionPointerType.h"
#include "nodes/NSizeOfOperator.h"
#include "nodes/NIdentifier.h"
#include "nodes/NIfStatement.h"
#include "nodes/NInteger.h"
#include "nodes/NMethodCall.h"
#include "nodes/Node.h"
#include "nodes/NReturnStatement.h"
#include "nodes/NStatement.h"
#include "nodes/NString.h"
#include "nodes/NStructureDeclaration.h"
#include "nodes/NStructureResolutionOperator.h"
#include "nodes/NType.h"
#include "nodes/NVariableDeclaration.h"
#include "nodes/NWhileStatement.h"

#endif