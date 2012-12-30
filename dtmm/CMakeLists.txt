cmake_minimum_required(VERSION 2.8.5)
project(dtmm)

add_executable(dtmm 
    main.c
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${curl_SOURCE_DIR}
)
target_link_libraries(dtmm
    argtable2
    bstring
    curl
    simclist
    lua
    ${curl_LIBRARY}
    libdcpu
)
cotire(dtmm)

if(${DEPLOYABLE})
    install(TARGETS dtmm
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

