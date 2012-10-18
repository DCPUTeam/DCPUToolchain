cmake_minimum_required(VERSION 2.8.5)
project(ltestdcpu)

add_executable(ltestdcpu
    bcopy-01.c
    bread-01.c
    bopen-02.c
    bread-02.c
    bopen-01.c
    btell-02.c
    tests.h
    main.c
    btell-01.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)
target_link_libraries(ltestdcpu
    argtable2
    bstring
    libdcpu
)

enable_testing()
add_test(bopen-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu bopen-01)
add_test(bopen-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu bopen-02)
add_test(bread-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu bread-01)
add_test(bread-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu bread-02)
# FIXME: This is a poorly written test.
#add_test(bcopy-01 ${OUTPUT_DIR}/ltestdcpu bcopy-01)
add_test(btell-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu btell-01)
add_test(btell-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu btell-02)

file(COPY empty.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
file(COPY test.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
file(COPY copy.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR} USE_SOURCE_PERMISSIONS)
