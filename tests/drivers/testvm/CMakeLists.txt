cmake_minimum_required(VERSION 2.8.5)
project(testvm)

add_executable(testvm
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)
target_link_libraries(testvm
    argtable2
    simclist
    bstring
    libdcpu
)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/vm")

# Define a function to easily create unit tests.
function(add_unit_test tname)
    get_property(dtasm_loc TARGET dtasm PROPERTY LOCATION)
    get_property(dtdb_loc TARGET dtdb PROPERTY LOCATION)
    string(REPLACE "$(Configuration)" "${CMAKE_BUILD_TYPE}" dtasm_loc "${dtasm_loc}")
    string(REPLACE "$(Configuration)" "${CMAKE_BUILD_TYPE}" dtdb_loc "${dtdb_loc}")
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testvm -a "${dtasm_loc}" -d "${dtdb_loc}" -m "${CMAKE_CURRENT_SOURCE_DIR}/modules" "${BASE_DIR}/${tname}.i")
endfunction(add_unit_test tname)

# Define the tests.
enable_testing()
add_unit_test(math-add)
add_unit_test(math-sub)
add_unit_test(math-mul)
add_unit_test(math-div)
add_unit_test(math-shr)
add_unit_test(math-mod)
# FIXME: These tests are broken.
#add_unit_test(math-mli)
#add_unit_test(math-dvi)
#add_unit_test(math-mdi)
add_unit_test(general-pick)
