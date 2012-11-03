cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-lang)

add_library(libdcpu-ci-lang ${LIBRARY_FORMAT}
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
set_target_properties(libdcpu-ci-lang PROPERTIES PREFIX "")
cotire(libdcpu-ci-lang)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-ci-lang
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} DESTINATION include/dcpu/ci/lang)
endif()
