cmake_minimum_required(VERSION 2.8.5)
project(sdl)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define linking information for each platform.
set(${PROJECT_NAME}_Unix SDL PARENT_SCOPE)
set(${PROJECT_NAME}_UnixNoInclude true PARENT_SCOPE)

# Define the sdl project.
message("  >> ${PROJECT_NAME}: prebuilt")
find_package(SDL REQUIRED)
if(NOT SDL_FOUND)
	message(FATAL_ERROR "    .. sdl was not found.  please install development headers!")
endif(NOT SDL_FOUND)
