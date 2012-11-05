cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-lang)

add_library(libdcpu-ci-lang STATIC
    AsmBlock.cpp
    AsmBlock.h
    AsmGenerator.cpp
    AsmGenerator.h
    Assembler.cpp
    Assembler.h
    ErrorList.cpp
    ErrorList.h
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

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)

cotire(libdcpu-ci-lang)
