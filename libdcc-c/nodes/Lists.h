/**

	File:			Lists.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares lists that are common to all
					AST nodes.

**/

#ifndef __DCPU_COMP_NODES_LISTS_H
#define __DCPU_COMP_NODES_LISTS_H

#include <string>
#include <vector>

class NStatement;
class NExpression;
class NDeclaration;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NDeclaration*> DeclarationList;
typedef std::vector<NVariableDeclaration*> VariableList;

#include "NStatement.h"
#include "NExpression.h"
#include "NDeclaration.h"
#include "NVariableDeclaration.h"

#endif