SET(COMPILER_SRC asmgen.cpp  asmtypes.cpp  lexer.cpp  Main.cpp  parser.cpp
    pp.c
    nodes/NAddressOfOperator.cpp    nodes/NExpressionStatement.cpp  nodes/NReturnStatement.cpp
    nodes/NAssignment.cpp           nodes/NForStatement.cpp         nodes/NSizeOfOperator.cpp
    nodes/NBinaryOperator.cpp       nodes/NFunctionDeclaration.cpp  nodes/NString.cpp
    nodes/NBlock.cpp                nodes/NFunctionSignature.cpp    nodes/NStructureDeclaration.cpp
    nodes/NCharacter.cpp            nodes/NIdentifier.cpp           nodes/NStructureResolutionOperator.cpp
    nodes/NDebugStatement.cpp       nodes/NIfStatement.cpp          nodes/NType.cpp
    nodes/NDeclarations.cpp         nodes/NInteger.cpp              nodes/NVariableDeclaration.cpp
    nodes/NDereferenceOperator.cpp  nodes/NMethodCall.cpp           nodes/NWhileStatement.cpp
    )

add_executable(compiler ${COMPILER_SRC})
target_link_libraries(compiler argtable2)
