cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-lang)

add_library(libdcpu-ci-lang STATIC
	AsmBlock.cpp
	AsmBlock.h
	AsmGenerator.cpp
	AsmGenerator.h
	Assembler.cpp
	Assembler.h
	CompilerException.h
	nodes/IDeclarations.h
	nodes/IFunctionDeclaration.h
	nodes/IFunctionSignature.h
	nodes/IType.h
	StackFrame.cpp
	StackFrame.h
	StackMap.h
	TypePosition.cpp
	TypePosition.h
)

