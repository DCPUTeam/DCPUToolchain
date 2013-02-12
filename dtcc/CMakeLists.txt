cmake_minimum_required(VERSION 2.8.5)
project(dtcc)

add_executable(dtcc
    main.cpp
)

message("cc include dir ${libdcpu-cc_SOURCE_DIR}")

include_directories(
    ${bstring_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
    ${libdcpu-cc_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(dtcc
    argtable2
    bstring
    libdcpu
    libdcpu-pp
    libdcpu-cc
)
cotire(dtcc)

if(${DEPLOYABLE})
    install(TARGETS dtcc
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()
