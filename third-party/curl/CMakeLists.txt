cmake_minimum_required(VERSION 2.8.5)
project(curl)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define linking information for each platform.
set(${PROJECT_NAME}_Windows libcurl PARENT_SCOPE) 
set(${PROJECT_NAME}_WindowsNoInclude false PARENT_SCOPE)
set(${PROJECT_NAME}_Cygwin /usr/lib/libcurl.dll.a PARENT_SCOPE)
set(${PROJECT_NAME}_CygwinNoInclude true PARENT_SCOPE)
set(${PROJECT_NAME}_Unix curl PARENT_SCOPE) 
set(${PROJECT_NAME}_UnixNoInclude true PARENT_SCOPE)

# Define the zlib project.
message("  >> ${PROJECT_NAME}: prebuilt")
if(MSVC)
	file(GLOB_RECURSE libcurl_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
	foreach(i ${libcurl_to_copy})
		file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
	endforeach(i ${libcurl_to_copy})
endif(MSVC)

