/**

	File:			Node.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the Node AST class.

**/

#ifndef __DCPU_COMP_NODES_NODE_H
#define __DCPU_COMP_NODES_NODE_H

class AsmBlock;
class AsmGenerator;

#include <string>

class Node
{
public:
	std::string cType;
protected:
	Node(std::string type) { this->cType = type; }
public:
	virtual ~Node() {}
	virtual AsmBlock* compile(AsmGenerator& context) = 0;
	virtual AsmBlock* reference(AsmGenerator& context) = 0;
};

#endif