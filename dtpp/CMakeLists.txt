cmake_minimum_required(VERSION 2.8.5)
project(dtpp)

add_executable(dtpp
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
    ${libdcpu-pp_BINARY_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(dtpp
    argtable2
    bstring
    simclist
    libdcpu
    libdcpu-pp
    libdcpu-pp-expr
)

if(${DEPLOYABLE})
    install(TARGETS dtpp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

