cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-lang-c)

bison_target(libdcpu-ci-lang-c-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "-d -y")
flex_target(libdcpu-ci-lang-c-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.hpp\")
add_flex_bison_dependency(libdcpu-ci-lang-c-lexer libdcpu-ci-lang-c-parser)
if(NOT MSVC AND NOT APPLE)
    set_source_files_properties(${FLEX_libdcpu-ci-lang-c-lexer_OUTPUTS} ${BISON_libdcpu-ci-lang-c-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-dangling-else)
endif()


include_directories(${libdcpu_SOURCE_DIR})
target_link_libraries(libdcpu)

add_library(libdcpu-ci-lang-c ${LIBRARY_FORMAT}
    nodes/Lists.h
    nodes/NAddressOfOperator.cpp
    nodes/NAddressOfOperator.h
    nodes/NArrayAccessOperator.cpp
    nodes/NArrayAccessOperator.h
    nodes/NArrayDeclaration.cpp
    nodes/NArrayDeclaration.h
    nodes/NAssemblyStatement.cpp
    nodes/NAssemblyStatement.h
    nodes/NAssignment.cpp
    nodes/NAssignment.h
    nodes/NBinaryOperator.cpp
    nodes/NBinaryOperator.h
    nodes/NBlock.cpp
    nodes/NBlock.h
    nodes/NBreakStatement.cpp
    nodes/NBreakStatement.h
    nodes/NBuiltInVaStart.cpp
    nodes/NBuiltInVaStart.h
    nodes/NCaseStatement.cpp
    nodes/NCaseStatement.h
    nodes/NCharacter.cpp
    nodes/NCharacter.h
    nodes/NContinueStatement.cpp
    nodes/NContinueStatement.h
    nodes/NDeclaration.h
    nodes/NDeclarations.cpp
    nodes/NDeclarations.h
    nodes/NDefaultStatement.cpp
    nodes/NDefaultStatement.h
    nodes/NDereferenceOperator.cpp
    nodes/NDereferenceOperator.h
    nodes/NEmptyStatement.cpp
    nodes/NEmptyStatement.h
    nodes/NExplicitCastOperator.cpp
    nodes/NExplicitCastOperator.h
    nodes/NExpression.h
    nodes/NExpressionStatement.cpp
    nodes/NExpressionStatement.h
    nodes/NForStatement.cpp
    nodes/NForStatement.h
    nodes/NFunctionDeclaration.cpp
    nodes/NFunctionDeclaration.h
    nodes/NFunctionPointerType.h
    nodes/NFunctionSignature.cpp
    nodes/NFunctionSignature.h
    nodes/NIdentifier.cpp
    nodes/NIdentifier.h
    nodes/NIfStatement.cpp
    nodes/NIfStatement.h
    nodes/NIntegerLiteral.cpp
    nodes/NIntegerLiteral.h
    nodes/NMethodCall.cpp
    nodes/NMethodCall.h
    nodes/Node.cpp
    nodes/Node.h
    nodes/NPostIncDec.cpp
    nodes/NPostIncDec.h
    nodes/NPreIncDec.cpp
    nodes/NPreIncDec.h
    nodes/NReturnStatement.cpp
    nodes/NReturnStatement.h
    nodes/NSizeOfOperator.cpp
    nodes/NSizeOfOperator.h
    nodes/NStatement.h
    nodes/NString.cpp
    nodes/NString.h
    nodes/NStructureDeclaration.cpp
    nodes/NStructureDeclaration.h
    nodes/NStructureResolutionOperator.cpp
    nodes/NStructureResolutionOperator.h
    nodes/NSwitchStatement.cpp
    nodes/NSwitchStatement.h
    nodes/NType.cpp
    nodes/NType.h
    nodes/NUnaryOperator.cpp
    nodes/NUnaryOperator.h
    nodes/NVariableDeclaration.cpp
    nodes/NVariableDeclaration.h
    nodes/NWhileStatement.cpp
    nodes/NWhileStatement.h
    nodes/TArrayMemArea.cpp
    nodes/TArrayMemArea.h
    nodes/TGenericBasicType.cpp
    nodes/TGenericBasicType.h
    nodes/TGenericInt16.cpp
    nodes/TGenericInt16.h
    nodes/TInt16.cpp
    nodes/TInt16.h
    nodes/TPointer16.cpp
    nodes/TPointer16.h
    nodes/TSignedInt16.cpp
    nodes/TSignedInt16.h
    nodes/TStruct.cpp
    nodes/TStruct.h
    nodes/TUnsignedInt16.cpp
    nodes/TUnsignedInt16.h
    ${FLEX_libdcpu-ci-lang-c-lexer_OUTPUTS}
    ${BISON_libdcpu-ci-lang-c-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-ci-lang_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
set_target_properties(libdcpu-ci-lang-c PROPERTIES PREFIX "")
cotire(libdcpu-ci-lang-c)

if(${DEPLOYABLE})
    target_link_libraries(libdcpu-ci-lang-c
        libdcpu-ci-lang
    )
    install(TARGETS libdcpu-ci-lang-c
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers_nodes "${CMAKE_CURRENT_SOURCE_DIR}/nodes/*.h")
    file(GLOB headers_gen "${CMAKE_CURRENT_BINARY_DIR}/*.hpp")
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} ${headers_gen} DESTINATION include/dcpu/ci/lang/c)
    install(FILES ${headers_nodes} DESTINATION include/dcpu/ci/lang/c/nodes)
endif()
