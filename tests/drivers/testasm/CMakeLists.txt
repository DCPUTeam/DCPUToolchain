cmake_minimum_required(VERSION 2.8.5)
project(testasm)

add_executable(testasm
    main.c
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)
target_link_libraries(testasm
    argtable2
    simclist
    bstring
    libdcpu
)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/asm")

# Determine linker path.
get_property(dtasm_loc TARGET dtasm PROPERTY LOCATION)
string(REPLACE "$(Configuration)" "${CMAKE_BUILD_TYPE}" dtasm_loc "${dtasm_loc}")

# Define a function to easily create assembler
# tests.
function(add_assemble_test tname input)
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testasm -p "${dtasm_loc}" -e "${BASE_DIR}/${tname}.oe" -a "${BASE_DIR}/${tname}.oa" "${BASE_DIR}/${input}.i")
endfunction(add_assemble_test tname)
function(add_assemble_fail tname)
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testasm -p "${dtasm_loc}" -f -a "${BASE_DIR}/${tname}.oa" "${BASE_DIR}/${input}.i")
endfunction(add_assemble_fail tname)
function(add_assemble_little tname input)
    add_test(${tname} ${CMAKE_CURRENT_BINARY_DIR}/testasm -p "${dtasm_loc}" --little-endian -e "${BASE_DIR}/${tname}.oe" -a "${BASE_DIR}/${tname}.oa" "${BASE_DIR}/${input}.i")
endfunction(add_assemble_little tname)

# Define the tests.
enable_testing()
add_assemble_test(stress-00 stress-00)
add_assemble_test(stress-01 stress-01)
add_assemble_test(stress-02 stress-02)
add_assemble_test(stress-03 stress-03)
add_assemble_test(stress-04 stress-04)
add_assemble_test(stress-05 stress-05)
add_assemble_test(stress-06 stress-06)
add_assemble_test(stress-07 stress-07)
add_assemble_test(endian-big endian)
add_assemble_little(endian-little endian)
add_assemble_fail(crash-paramcount)
