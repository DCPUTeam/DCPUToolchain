/**

    File:           SymbolTypes.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the Symbol Types for the SymbolTable.

**/
#ifndef __DCPU_LIBDCC_SYMBOLTYPES_H
#define __DCPU_LIBDCC_SYMBOLTYPES_H

enum DeclType {VARIABLE_DECL, FUNCTION_DECL, STRUCT_DECL};
enum ObjectPosition {GLOBAL, LOCAL_STACK, PARAMETER_STACK};

#endif
