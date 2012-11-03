cmake_minimum_required(VERSION 2.8.5)
project(dtlua)

add_executable(dtlua
    lua.c
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${lua_SOURCE_DIR}
)
target_link_libraries(dtlua
    argtable2
    bstring
    lua
    libdcpu
)
cotire(dtlua)

if(${DEPLOYABLE})
    install(TARGETS dtlua
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

