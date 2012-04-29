cmake_minimum_required(VERSION 2.8.5)
project(ddb)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)


# Define the emulator project.
dt_project(executable)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
dt_needs_prebuilt(readline readline readline)
dt_needs_prebuilt(pthread pthreadVC2 pthread)  	
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
find_library(readline readline readline)
find_library(pthread pthreadVC2 pthread)
target_link_libraries(ddb readline)
target_link_libraries(ddb pthread)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

dt_needs(bstring)
dt_needs(libdcpu-vm-dbg)
dt_needs(libdcpu-vm)
dt_needs(libdcpu)

if(DEFINED EXPERIMENTAL)
	add_definitions(-DFEATURE_SDP)
endif(DEFINED EXPERIMENTAL)
