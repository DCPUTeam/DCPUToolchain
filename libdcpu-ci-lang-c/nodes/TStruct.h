/**

	File:		TStruct.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TStruct Type class.

**/


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
		bool m_isConst;
		bool m_isSigned;
		std::string m_name;
		NStructureDeclaration* m_resolvedStruct;
		AsmGenerator* m_context;

		void resolveStruct();
	public:
		virtual std::string getName() const;
		virtual std::string getInternalName() const;

		bool isBasicType() const;
		bool isStruct() const;
		bool isPointer() const;

		bool isConst() const;
		void setConst();
		bool isSigned() const
		{
			throw new CompilerException(0, "<internal>",
						    "Signedness not defined for structs (internal error).");
		}
		bool setSigned(bool isSigned)
		{
			throw new CompilerException(0, "<internal>",
						    "Signedness not defined for structs (internal error).");
		}


		/* cast operations */
		virtual bool implicitCastable(AsmGenerator& context, const IType* toType);
		virtual bool explicitCastable(AsmGenerator& context, const IType* toType);
		virtual AsmBlock* implicitCast(AsmGenerator& context, const IType* toType, char a);
		virtual AsmBlock* explicitCast(AsmGenerator& context, const IType* toType, char a);


		// struct specific functions and members

		uint16_t getStructFieldPosition(std::string name);
		IType* getStructFieldType(std::string name);
		size_t getBitSize();

		// a struct is context dependent
		TStruct(const std::string& name) : m_name(name), m_resolvedStruct(NULL), m_context(NULL) { }

		void initContext(AsmGenerator& context);

		virtual uint16_t getWordSize();
		virtual uint16_t getWordSize(AsmGenerator& context);

		/* copy */
		// direct copy via registers
		virtual AsmBlock* copyValue(AsmGenerator& context, char from, char to);
		// indirect copy given references (copies values)
		virtual AsmBlock* copyByRef(AsmGenerator& context, char fromRef, char toRef);
		// saves value in "from" register into the reference
		virtual AsmBlock*  saveToRef(AsmGenerator& context, char from, char toRef);
		// load from a reference into a value
		virtual AsmBlock*  loadFromRef(AsmGenerator& context, char fromRef, char to);

		/* stack ops */
		virtual AsmBlock* pushStack(AsmGenerator& context, char a);
		// FIXME do i need this?
		// virtual AsmBlock* pushStackByRef(char a);
		virtual AsmBlock* popStack(AsmGenerator& context);
		virtual AsmBlock* popStackReturn(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Stack pop with return not permitted for structs (internal error).");
		}
		// for debugging, cleaning the stack while pop
		virtual AsmBlock* popStackClean(AsmGenerator& context);
		virtual AsmBlock* popStackCleanReturn(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Stack pop with return not permitted for structs (internal error).");
		}



		/* binary operators */
		virtual AsmBlock* add(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* sub(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* band(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bxor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* shl(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* shr(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* land(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lor(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}

		virtual AsmBlock* mul(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* div(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* mod(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}


		/* unary operators */
		virtual AsmBlock* plus(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* minus(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* bnot(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lnot(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* inc(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* dec(AsmGenerator& context, char a)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}

		/* comparison operators */
		virtual AsmBlock* eq(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* neq(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}

		virtual AsmBlock* gt(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* lt(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* ge(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
		virtual AsmBlock* le(AsmGenerator& context, char a, char b)
		{
			throw new CompilerException(0, "<internal>",
						    "Operation not permitted for structs (internal error).");
		}
};
#endif

