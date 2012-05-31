cmake_minimum_required(VERSION 2.8.5)
project(testld)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/ld")

# Define the assembler project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(simclist)
dt_needs(bstring)
dt_needs(libdcpu)

# Define a function to easily create linker
# tests.
function(add_link_test tname)
	set(files "")
	foreach(i IN LISTS ARGN)
		list(APPEND files "${BASE_DIR}/${i}.i")
	endforeach(i IN LISTS ARGN)
	add_test(${tname} ${OUTPUT_DIR}/testld -vvv -e "${BASE_DIR}/${tname}.oe" -a "${BASE_DIR}/${tname}.oa" ${files})
endfunction(add_link_test tname)
function(add_link_fail tname)
	set(files "")
	foreach(i IN LISTS ARGN)
		list(APPEND files "${BASE_DIR}/${i}.i")
	endforeach(i IN LISTS ARGN)
	add_test(${tname} ${OUTPUT_DIR}/testld -vvv -f -a "${BASE_DIR}/${tname}.oa" ${files})
endfunction(add_link_fail tname)

# Define the tests.
enable_testing()
add_link_test(plain-01 plain-01)
add_link_fail(import-01 import-01)
add_link_test(import-02 import-02)
add_link_fail(import-03 import-03)
add_link_test(import-04 import-04)

