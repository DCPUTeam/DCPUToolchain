cmake_minimum_required(VERSION 2.8.5)
project(testld)

add_executable(testld
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)
target_link_libraries(testld
    argtable2
    simclist
    bstring
    libdcpu
)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/ld")

# Determine linker path.
get_property(dtld_loc TARGET dtld PROPERTY LOCATION)
string(REPLACE "$(Configuration)" "${CMAKE_BUILD_TYPE}" dtld_loc "${dtld_loc}")

# Define a function to easily create linker
# tests.
function(add_link_test tname)
    set(files "")
    foreach(i IN LISTS ARGN)
        list(APPEND files "${BASE_DIR}/${i}.i")
    endforeach(i IN LISTS ARGN)
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testld -p "${dtld_loc}" -vvv -e "${BASE_DIR}/${tname}.oe" -a "${BASE_DIR}/${tname}.oa" ${files})
endfunction(add_link_test tname)
function(add_link_fail tname)
    set(files "")
    foreach(i IN LISTS ARGN)
        list(APPEND files "${BASE_DIR}/${i}.i")
    endforeach(i IN LISTS ARGN)
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testld -p "${dtld_loc}" -vvv -f -a "${BASE_DIR}/${tname}.oa" ${files})
endfunction(add_link_fail tname)

# Define the tests.
enable_testing()
add_link_test(plain-01 plain-01)
add_link_fail(import-01 import-01)
add_link_test(import-02 import-02)
add_link_fail(import-03 import-03)
add_link_test(import-04 import-04)

