/**

	File:		Node.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Declares the Node AST class.

**/

#ifndef __DCPU_COMP_NODES_NODE_H
#define __DCPU_COMP_NODES_NODE_H

class AsmBlock;
class AsmGenerator;

#include <string>

extern int yylineno;
extern "C"
{
#include <bstring.h>
	extern bstring yyfilename;
}

class Node
{
	private:
		int line;
		std::string file;
	public:
		std::string cType;
	protected:
		Node(std::string type)
		{
			this->cType = type;
			this->line = yylineno;
			if (yyfilename == NULL)
				this->file = "<unknown>";
			else
				this->file = std::string((const char*)yyfilename->data);
		}
	public:
		virtual ~Node() {}
		virtual AsmBlock* compile(AsmGenerator& context) = 0;
		virtual AsmBlock* reference(AsmGenerator& context) = 0;
		AsmBlock getFileAndLineState() const;
};

#endif
