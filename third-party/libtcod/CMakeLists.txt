cmake_minimum_required(VERSION 2.8.5)
project(libtcod)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Depending on the platform, we either need to compile
# this library ourselves, or use the libraries that are
# already provided.
if(MSVC OR TOOLCHAIN_ON_CYGWIN)
	# Define a prebuilt project.
	message("  >> ${PROJECT_NAME}: prebuilt")
	add_library(libtcod STATIC IMPORTED)
	set(IMPORTED_LOCATION ./lib)
	set_property(TARGET libtcod PROPERTY include_path "${CMAKE_CURRENT_SOURCE_DIR}")
	
	# Define linking information for each platform.
	set(${PROJECT_NAME}_Windows libtcod-VS PARENT_SCOPE)

	# Copy supporting files.
	file(GLOB_RECURSE libtcod_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
	foreach(i ${libtcod_to_copy})
		file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
	endforeach(i ${libtcod_to_copy})
else(MSVC OR TOOLCHAIN_ON_CYGWIN)
	# Define a project we will compile ourselves.
	dt_project(library)
	dt_needs_prebuilt(zlib)
	dt_needs_prebuilt(libpng)
	dt_needs_prebuilt(sdl)
	dt_needs_prebuilt(opengl)
	dt_folder("third-party")	

	# OS X
	dt_needs_osx_dependency(SDL)
	dt_needs_osx_dependency(SDLmain)

	# We also need to change our C compiler flags, since TCOD doesn't
	# conform to our compiling standards :P
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error -Wno-declaration-after-statement")

	# Copy supporting files.
	file(GLOB_RECURSE libtcod_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
	foreach(i ${libtcod_to_copy})
		file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
	endforeach(i ${libtcod_to_copy})
endif(MSVC OR TOOLCHAIN_ON_CYGWIN)

