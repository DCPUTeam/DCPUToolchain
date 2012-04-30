cmake_minimum_required(VERSION 2.8.5)
project(compiler)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the compiler project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(libdcpu)
dt_needs(libdcpu-ci-lang)
dt_needs(libdcpu-ci-lang-c)

# Copy required data files.
file(COPY ./bootstrap.asm DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)

