cmake_minimum_required (VERSION 2.6)
project(libtcod)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the libtcod project.
message("  >> ${PROJECT_NAME}: prebuilt")
add_library(libtcod STATIC IMPORTED)
set(IMPORTED_LOCATION ./lib)

