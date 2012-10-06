cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm-python)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the pydcpu project.
dt_project(library)
dt_needs(libdcpu-vm)
dt_needs(libdcpu)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )
include(UseCython)

add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake
	${CMAKE_CURRENT_BINARY_DIR}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

cython_add_module(dcpu dcpu.pyx)
target_link_libraries(dcpu libdcpu)
target_link_libraries(dcpu libdcpu-vm)
