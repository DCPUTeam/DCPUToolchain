cmake_minimum_required(VERSION 2.8.5)
project(libpng)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define linking information for each platform.
set(${PROJECT_NAME}_Emscripten libpng PARENT_SCOPE)
set(${PROJECT_NAME}_EmscriptenNoInclude true PARENT_SCOPE)
set(${PROJECT_NAME}_Unix png PARENT_SCOPE) 
set(${PROJECT_NAME}_UnixNoInclude true PARENT_SCOPE)

# Define the libpng project.
if(EMSCRIPTEN)
	message("  >> ${PROJECT_NAME}: emscripten")
else(EMSCRIPTEN)
	message("  >> ${PROJECT_NAME}: prebuilt")
	find_package(PNG REQUIRED)
endif(EMSCRIPTEN)
