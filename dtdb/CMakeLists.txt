cmake_minimum_required(VERSION 2.8.5)
project(dtdb)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)


# Define the emulator project.
dt_project(executable)
dt_needs_prebuilt(readline)
dt_needs_prebuilt(pthread)
dt_needs(argtable2)
dt_needs(bstring)
dt_needs(libdcpu-ci-dbgfmt)
dt_needs(libdcpu-vm-dbg)
dt_needs(libdcpu-vm)
dt_needs(libdcpu)

if(DEFINED EXPERIMENTAL)
	add_definitions(-DFEATURE_SDP)
endif(DEFINED EXPERIMENTAL)
