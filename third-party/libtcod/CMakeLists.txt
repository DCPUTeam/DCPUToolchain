cmake_minimum_required(VERSION 2.8.5)
project(libtcod)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the libtcod project.
dt_project(library)
dt_needs_prebuilt(zlib)
dt_needs_prebuilt(libpng)
dt_needs_prebuilt(sdl)
dt_needs_prebuilt(opengl)
dt_folder("third-party")

# We also need to change our C compiler flags, since TCOD doesn't
# conform to our compiling standards :P
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
elseif(APPLE)
	# Reset XCode compiler flags.
else(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error -Wno-declaration-after-statement")
endif(MSVC)

# Copy supporting files.
file(GLOB_RECURSE libtcod_to_copy RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/bin" bin/*)
foreach(i ${libtcod_to_copy})
	file(COPY ./bin/${i} DESTINATION "${ROOT_DIR}/Debug" USE_SOURCE_PERMISSIONS)
endforeach(i ${libtcod_to_copy})
