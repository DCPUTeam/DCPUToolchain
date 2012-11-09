cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-lang)

add_library(libdcpu-ci-lang ${LIBRARY_FORMAT}
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

set_target_properties(libdcpu-ci-lang PROPERTIES PREFIX "")
cotire(libdcpu-ci-lang)

if(${DEPLOYABLE})
    target_link_libraries(libdcpu-ci-lang
        libdcpu
    )
    install(TARGETS libdcpu-ci-lang
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} DESTINATION include/dcpu/ci/lang)
endif()
