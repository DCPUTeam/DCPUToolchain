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
    nodes/IDeclaration.h
    nodes/IDeclarations.h
    nodes/IFunctionDeclaration.h
    nodes/IFunctionSignature.h
    nodes/IType.h
    StackMap.h
    TypePosition.cpp
    TypePosition.h
    SymbolTable.cpp
    SymbolTable.h
    SymbolTableScope.cpp
    SymbolTableScope.h
    SymbolObject.cpp
    SymbolObject.h
    SymbolTypes.h
)
cotire(libdcpu-ci-lang)
