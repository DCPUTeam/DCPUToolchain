cmake_minimum_required(VERSION 2.8.5)
project(dtld)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the linker project.
dt_project(executable)
dt_needs(bstring)
dt_needs(simclist)
dt_needs(argtable2)
dt_needs(lua)
dt_needs(libdcpu)
dt_needs(libdcpu-pp-expr)
dt_needs(libdcpu-ci-objfmt)
dt_needs(libdcpu-ci-dbgfmt)
