cmake_minimum_required(VERSION 2.8.5)
project(emulator)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)


# Define the emulator project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(libdcpu)
dt_needs(libdcpu-vm)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	dt_needs_prebuilt(libtcod libtcod-VS)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_library(tcod tcod)
	target_link_libraries(emulator tcod)
	# if library is not installed
	#dt_needs_prebuilt(libtcod libtcod-VS)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
