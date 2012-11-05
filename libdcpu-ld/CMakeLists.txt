cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ld)

add_library(libdcpu-ld ${LIBRARY_FORMAT}
    ldbin.c
    ldbin.h
    ldbins.c
    ldbins.h
    ldconv.c
    ldconv.h
    ldlua.c
    ldlua.h
    ldpolicy.c
    ldpolicy.h
    ldcall.c
    ldcall.h
    ccstackcall.h
    ccstackcall.c
    ccregistercall.h
    ccregistercall.c
    ccregisterdirect.h
    ccregisterdirect.c
)
include_directories(
    ${libdcpu-ld-policy_SOURCE_DIR}
    ${libdcpu-ld-policy_BINARY_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${lua_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-vm_SOURCE_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${libdcpu-ci-objfmt_SOURCE_DIR}
    ${libdcpu-ci-dbgfmt_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(libdcpu-ld
    bstring
    simclist
    lua
    libdcpu-vm
    libdcpu
    libdcpu-pp-expr
    libdcpu-ci-objfmt
    libdcpu-ci-dbgfmt
    libdcpu-ld-policy
)
cotire(libdcpu-ld)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-ld
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} ${headers_gen} DESTINATION include/dcpu/ld)
endif()

