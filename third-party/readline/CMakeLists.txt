cmake_minimum_required(VERSION 2.8.5)
project(readline)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define linking information for each platform.
set(${PROJECT_NAME}_Windows readline PARENT_SCOPE)
set(${PROJECT_NAME}_Unix readline PARENT_SCOPE)
set(${PROJECT_NAME}_UnixNoInclude true PARENT_SCOPE)

# Define the readline project.
message("  >> ${PROJECT_NAME}: prebuilt")
add_library(readline SHARED IMPORTED)
set(IMPORTED_LOCATION ./lib)
set_property(TARGET readline PROPERTY include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Copy supporting files.
file(GLOB_RECURSE readline_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
foreach(i ${readline_to_copy})
	file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
endforeach(i ${readline_to_copy})
