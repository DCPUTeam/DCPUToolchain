/**

	File:		AsmGenerator.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the AsmGenerator class.

**/

#ifndef __DCPU_LIBDCC_ASMGENERATOR_H
#define __DCPU_LIBDCC_ASMGENERATOR_H

class StackFrame;

#include <vector>
#include <string>
#include "Assembler.h"
#include "AsmBlock.h"
#include "StackFrame.h"
#include "nodes/IDeclarations.h"
#include "nodes/IFunctionDeclaration.h"
#include "nodes/IFunctionSignature.h"

class AsmGenerator
{
	private:
		const Assembler* m_AssemblerTarget;
		std::vector<std::string> m_AutomaticLabels;
		static char getRandomCharacter();
		static std::string getRandomString(std::string::size_type sz);

	public:
		StackFrame* m_CurrentFrame;
		StackFrame* m_GlobalFrame;
		IDeclarations* m_RootNode;

	public:
		AsmGenerator(std::string asmtarget);

		IFunctionDeclaration* getFunction(std::string name);
		StackFrame* generateStackFrame(IFunctionDeclaration* function, bool referenceOnly = true);
		StackFrame* generateStackFrameIncomplete(IFunctionSignature* signature);
		void finishStackFrame(StackFrame* frame);
		std::string getRandomLabel(std::string prefix);
		inline const Assembler& getAssembler()
		{
			return *(this->m_AssemblerTarget);
		}
		inline bool isAssemblerDebug()
		{
			return true;
		}
};

#endif
