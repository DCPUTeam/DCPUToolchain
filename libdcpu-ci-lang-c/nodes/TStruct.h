#ifndef __DCPU_COMP_TYPES_STRUCT_H
#define __DCPU_COMP_TYPES_STRUCT_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>
#include "NStructureDeclaration.h"

class TStruct: public IType
{
	private:
		std::string m_name;
		NStructureDeclaration* m_resolvedStruct;
		AsmGenerator* m_context;
	
		void resolveStruct();
	public:
		// struct specific functions and members
		
		uint16_t getStructFieldPosition(std::string name);
		size_t getBitSize();
		
		// a struct is context dependent
		TStruct(const std::string& name) : m_name(name), m_resolvedStruct(NULL), m_context(NULL) { }
		
		void initContext(AsmGenerator& context);
		
		virtual uint16_t getWordSize();
		virtual uint16_t getWordSize(AsmGenerator& context);
		
		/* copy */
		// direct copy via registers
		virtual AsmBlock* copyValue(char from, char to);
		// indirect copy given references (copies values)
		virtual AsmBlock* copyByRef(char fromRef, char toRef);
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(char from, char toRef);
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(char fromRef, char to);
		
		/* stack ops */
		virtual AsmBlock* pushStack(char a);
		// FIXME do i need this? 
		// virtual AsmBlock* pushStackByRef(char a);
		virtual AsmBlock* popStack();
		virtual AsmBlock* popStackReturn(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Stack pop with return not permitted for structs (internal error).");
		}
		// for debugging, cleaning the stack while pop
		virtual AsmBlock* popStackClean();
		virtual AsmBlock* popStackCleanReturn(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Stack pop with return not permitted for structs (internal error).");
		}
		
		
		
		/* binary operators */
		virtual AsmBlock* add(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* sub(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* band(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bor(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bxor(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* shl(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* shr(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* land(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lor(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
				
		virtual AsmBlock* mul(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* div(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* mod(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		
		
		/* unary operators */
		virtual AsmBlock* plus(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* minus(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bnot(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lnot(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* inc(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* dec(char a)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		
		/* comparison operators */
		virtual AsmBlock* eq(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* neq(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		
		virtual AsmBlock* gt(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lt(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* ge(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* le(char a, char b)
		{
			throw new CompilerException(0, "<internal>", 
			"Operation not permitted for structs (internal error).");
		}
};
#endif

