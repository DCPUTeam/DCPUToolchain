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
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testvm -a "${dtasm_BINARY_DIR}/dtasm${CMAKE_EXECUTABLE_SUFFIX}" -d "${dtdb_BINARY_DIR}/dtdb${CMAKE_EXECUTABLE_SUFFIX}" -m "${toolchain_SOURCE_DIR}/modules" "${BASE_DIR}/${tname}.i")
endfunction(add_unit_test tname)

# Define the tests.
enable_testing()
add_unit_test(math-add)
add_unit_test(math-sub)
add_unit_test(math-mul)
add_unit_test(math-div)
add_unit_test(math-shr)
add_unit_test(general-pick)
