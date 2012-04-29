cmake_minimum_required(VERSION 2.8.5)
project(pthread)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the libtcod project.
message("  >> ${PROJECT_NAME}: prebuilt")
add_library(pthread SHARED IMPORTED)
set(IMPORTED_LOCATION ./lib)
set_property(TARGET pthread PROPERTY include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Copy supporting files.
file(GLOB_RECURSE pthread_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
foreach(i ${pthread_to_copy})
	file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
endforeach(i ${pthread_to_copy})
