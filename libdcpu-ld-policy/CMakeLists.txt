cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ld-policy)

bison_target(libdcpu-ld-policy-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(libdcpu-ld-policy-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h\")
add_flex_bison_dependency(libdcpu-ld-policy-lexer libdcpu-ld-policy-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_libdcpu-ld-policy-lexer_OUTPUTS} ${BISON_libdcpu-ld-policy-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()
add_library(libdcpu-ld-policy ${LIBRARY_FORMAT}
    policy.h
    policy.c
    policyast.h
    policyast.c
    policyexec.h
    policyexec.c
    ${FLEX_libdcpu-ld-policy-lexer_OUTPUTS}
    ${BISON_libdcpu-ld-policy-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(libdcpu-ld-policy
    bstring
    simclist
    libdcpu
)
set_target_properties(libdcpu-ld-policy PROPERTIES PREFIX "")
cotire(libdcpu-ld-policy)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-ld-policy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers_gen "${CMAKE_CURRENT_BINARY_DIR}/*.h")
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} ${headers_gen} DESTINATION include/dcpu/ld/policy)
endif()
