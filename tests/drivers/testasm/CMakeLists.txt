cmake_minimum_required(VERSION 2.8.5)
project(testasm)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/asm")

# Define the assembler project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(simclist)
dt_needs(bstring)
dt_needs(libdcpu)

# Define a function to easily create linker
# tests.
function(add_assemble_test tname input)
	add_test(${tname} ${OUTPUT_DIR}/testasm -e "${BASE_DIR}/${tname}.oe" -a "${BASE_DIR}/${tname}.oa" "${BASE_DIR}/${input}.i")
endfunction(add_assemble_test tname)
function(add_assemble_fail tname)
	add_test(${tname} ${OUTPUT_DIR}/testasm -f -a "${BASE_DIR}/${tname}.oa" "${BASE_DIR}/${input}.i")
endfunction(add_assemble_fail tname)

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

