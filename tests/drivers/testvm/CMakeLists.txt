cmake_minimum_required(VERSION 2.8.5)
project(testvm)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/vm")

# Define the testvm project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(simclist)
dt_needs(bstring)
dt_needs(libdcpu)

# Define a function to easily create unit tests.
function(add_unit_test tname)
	add_test(${tname} ${OUTPUT_DIR}/testvm "${BASE_DIR}/${tname}.i")
endfunction(add_unit_test tname)

# Define the tests.
enable_testing()
add_unit_test(math-shr)
add_unit_test(general-pick)
