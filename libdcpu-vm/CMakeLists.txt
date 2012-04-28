cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the libvm project.
dt_project(library)
dt_needs(libdcpu)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	dt_needs_prebuilt(libtcod libtcod-VS)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	dt_needs_prebuilt(libtcod libtcod-VS)
	find_library(tcod tcod)
	target_link_libraries(libdcpu-vm tcod)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

