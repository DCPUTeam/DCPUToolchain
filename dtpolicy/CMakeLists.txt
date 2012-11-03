cmake_minimum_required(VERSION 2.8.5)
project(dtpolicy)

add_executable(dtpolicy
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu-ld-policy_SOURCE_DIR}
)
target_link_libraries(dtpolicy
    argtable2
    bstring
    simclist
    libdcpu
    libdcpu-ld-policy
)
cotire(dtpolicy)

if(${DEPLOYABLE})
    install(TARGETS dtpolicy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

