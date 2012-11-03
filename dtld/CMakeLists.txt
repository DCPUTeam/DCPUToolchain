cmake_minimum_required(VERSION 2.8.5)
project(dtld)

add_executable(dtld
    ldbin.c
    ldbin.h
    ldbins.c
    ldbins.h
    ldconv.c
    ldconv.h
    ldlua.c
    ldlua.h
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
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
target_link_libraries(dtld
    argtable2
    bstring
    simclist
    lua
    libdcpu-vm
    libdcpu
    libdcpu-pp-expr
    libdcpu-ci-objfmt
    libdcpu-ci-dbgfmt
)
cotire(dtld)

if(${DEPLOYABLE})
    install(TARGETS dtld
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

