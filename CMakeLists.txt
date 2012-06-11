cmake_minimum_required(VERSION 2.8.5)
project (toolchain)

# Main version information.
set(VERSION_STAGE "beta")
execute_process(COMMAND git rev-parse HEAD OUTPUT_VARIABLE git_hash)
if("${git_hash}" STREQUAL "")
	add_definitions(-DVERSION_STAGE=${VERSION_STAGE})
else("${git_hash}" STREQUAL "")
	string(STRIP ${git_hash} git_hash)
	string(SUBSTRING ${git_hash} 0 9 git_hash)
	set(VERSION_GIT ${git_hash})
	add_definitions(-DVERSION_STAGE=${VERSION_STAGE} -DVERSION_GIT=${VERSION_GIT})
endif("${git_hash}" STREQUAL "")

# Set up options.
option(BUILD_EMULATOR "Build the emulator and debugger." ON)
option(BUILD_DOCUMENTATION "Build the documentation with Sphinx." ON)
option(BUILD_KERNEL "Build the kernel if available." OFF)
option(BUILD_STDLIB "Build the standard libraries." ON)
option(BUILD_MODULE_MANAGER "Build the module manager." ON)

# Set up some basics.
set(IS_ROOT 1)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeScripts")
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Debug")
endif(MSVC)
set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Debug")
set(FLEX_UNISTD_INCLUDE "")
set(CMAKE_BUILD_TYPE Debug)
if(MSVC)
	# Windows does not have unistd.h, but GnuWin32's Flex program
	# provides it for us, so we need to add that as an include
	# directory in this case.
	set(FLEX_UNISTD_INCLUDE "${ROOT_DIR}/third-party/flex")
endif(MSVC)
if(NOT DEFINED FLEX_EXECUTABLE)
	find_program(FLEX_EXECUTABLE NAMES flex.exe flex)
endif(NOT DEFINED FLEX_EXECUTABLE)
if(NOT DEFINED BISON_EXECUTABLE)
	find_program(BISON_EXECUTABLE NAMES bison.exe bison)
endif(NOT DEFINED BISON_EXECUTABLE)
if(NOT DEFINED M4_EXECUTABLE)
	find_program(M4_EXECUTABLE NAMES m4.exe m4)
endif(NOT DEFINED M4_EXECUTABLE)
if(DEFINED ENV{SHELL})
	if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		if(NOT MSVC)
			set(TOOLCHAIN_ON_CYGWIN true)
		endif(NOT MSVC)
	endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
endif(DEFINED ENV{SHELL})
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

# Set compiler options.
get_filename_component(CMAKE_C_COMPILER_BASENAME ${CMAKE_C_COMPILER} NAME_WE)
set(OUTPUT_EXT "")
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
elseif(APPLE)
	# TODO: Set warnings as errors under Mac / XCode.
elseif(MINGW)
	# This is a cross of other stuff in here; configured for building under unix, for windows :)
	# This method of compilation is extrememly advised against; it could work, but you are better off getting official
	# binaries for windows, ones that have been tested.
	# A major reason for this is because the -Werror flag has been removed.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement -Wno-int-to-pointer-cast -D SIMCLIST_NO_DUMPRESTORE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D SIMCLIST_NO_DUMPRESTORE") # TODO: Fix code to allow -Werror here.
elseif(${CMAKE_C_COMPILER_BASENAME} STREQUAL "clang" OR ${CMAKE_C_COMPILER_BASENAME} STREQUAL "emcc")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu89 -Werror -Wdeclaration-after-statement -Wno-unused-value -Wno-pointer-sign -Wno-int-to-pointer-cast -Wno-switch -Qunused-arguments")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # TODO: Fix code to allow -Werror here.
	if(${CMAKE_C_COMPILER_BASENAME} STREQUAL "emcc")
		add_definitions(-D__EMSCRIPTEN__ -DNO_OPENGL)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${ROOT_DIR}/html5/deploy --base-dir ${ROOT_DIR}/html5/deploy")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		set(OUTPUT_EXT ".html")
		set(EMSCRIPTEN true)
	endif(${CMAKE_C_COMPILER_BASENAME} STREQUAL "emcc")
else(MSVC)
	# Assume GNU since there doesn't appear to be
	# a better way of detecting this.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wdeclaration-after-statement -Wno-int-to-pointer-cast")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # TODO: Fix code to allow -Werror here.
endif(MSVC)

# Show configuration.
message("-- Showing configuration")
if(DEFINED VERSION_GIT)
	message("  >> version is: '${VERSION_STAGE}; #${VERSION_GIT}'")
else(DEFINED VERSION_GIT)
	message("  >> version is: '${VERSION_STAGE}'")
endif(DEFINED VERSION_GIT)
message("  >> m4 is at: ${M4_EXECUTABLE}")
message("  >> flex is at: ${FLEX_EXECUTABLE}")
message("  >> flex unistd is at: ${FLEX_UNISTD_INCLUDE}")
message("  >> bison is at: ${BISON_EXECUTABLE}")
message("   (must be Cygwin versions on Windows)")
if(NOT "${EXPERIMENTAL}" STREQUAL "")
message("  >> experimental features are enabled!")
set(EXPERIMENTAL true)
endif(NOT "${EXPERIMENTAL}" STREQUAL "")
if(DEFINED TOOLCHAIN_ON_CYGWIN)
message("  >> detected cygwin!")
endif(DEFINED TOOLCHAIN_ON_CYGWIN)

# Define a function that automatically includes
# all of the .h and .c files in the project folder.
function(dt_project type)
	# Initialize.
	set(${PROJECT_NAME}_all "")

	# Add all source and header files first.
	file(GLOB ${PROJECT_NAME}_source_c *.c)
	file(GLOB ${PROJECT_NAME}_source_cpp *.cpp)
	file(GLOB ${PROJECT_NAME}_header_c *.h)
	file(GLOB ${PROJECT_NAME}_header_cpp *.hpp)
	file(GLOB ${PROJECT_NAME}_parser_y parser.y)
	file(GLOB ${PROJECT_NAME}_parser_l lexer.l)

	# Check to see if we need to handle parsers.
	if("${${PROJECT_NAME}_parser_y}" MATCHES "^(.*)/parser.y$")
		string(LENGTH "${${PROJECT_NAME}_source_cpp}" has_cxx)
		if(${has_cxx} EQUAL 0)
			dt_has_parser(false)
		else(${has_cxx} EQUAL 0)
			dt_has_parser(true)
		endif(${has_cxx} EQUAL 0)
	endif("${${PROJECT_NAME}_parser_y}" MATCHES "^(.*)/parser.y$")

	# Output messages.
	if("${type}" MATCHES "^executable$")
		message("  >> ${PROJECT_NAME}: executable")
	elseif("${type}" MATCHES "^library$")
		message("  >> ${PROJECT_NAME}: library")
	else("${type}" MATCHES "^executable$")
		message(FATAL_ERROR "Unknown project type definition for ${PROJECT_NAME}")
	endif("${type}" MATCHES "^executable$")

	# Add any additional directories.
	if(NOT 1 EQUAL ${ARGC})
		math(EXPR ARGC_OFFSET "${ARGC} - 1")
		foreach(v RANGE 1 ${ARGC_OFFSET})
			set(i ${ARGV${v}})
			message("    .. including '${i}'")
	
			# Search for all C / C++ files in the directory.
			file(GLOB ${PROJECT_NAME}_${i}_source_c ${i}/*.c)
			file(GLOB ${PROJECT_NAME}_${i}_source_cpp ${i}/*.cpp)
			file(GLOB ${PROJECT_NAME}_${i}_header_c ${i}/*.h)
			file(GLOB ${PROJECT_NAME}_${i}_header_cpp ${i}/*.hpp)
			
			# Set groupings for IDEs.
			source_group("Source Files\\\\${i}" FILES ${${PROJECT_NAME}_${i}_source_c} ${${PROJECT_NAME}_${i}_source_cpp})
			source_group("Header Files\\\\${i}" FILES ${${PROJECT_NAME}_${i}_header_c} ${${PROJECT_NAME}_${i}_header_cpp})
			source_group("Lexer / Parser Files\\\\${i}" FILES ${${PROJECT_NAME}_${i}_parser_y} ${${PROJECT_NAME}_${i}_parser_l})
	
			# Append to source code list.
			set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_${i}_source_c} ${${PROJECT_NAME}_${i}_source_cpp} ${${PROJECT_NAME}_${i}_header_c} ${${PROJECT_NAME}_${i}_header_cpp})
		endforeach(v RANGE 1 ${ARGC})
	endif(NOT 1 EQUAL ${ARGC})

	# Join it all together.
	set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_source_c} ${${PROJECT_NAME}_source_cpp} ${${PROJECT_NAME}_header_c} ${${PROJECT_NAME}_header_cpp} ${${PROJECT_NAME}_parser_y} ${${PROJECT_NAME}_parser_l})

	# Set groupings for IDEs.
	source_group("Source Files" FILES ${${PROJECT_NAME}_source_c} ${${PROJECT_NAME}_source_cpp})
	source_group("Header Files" FILES ${${PROJECT_NAME}_header_c} ${${PROJECT_NAME}_header_cpp})
	source_group("Lexer / Parser Files" FILES ${${PROJECT_NAME}_parser_y} ${${PROJECT_NAME}_parser_l})

	# Add the flex unistd directory.
	include_directories(${FLEX_UNISTD_INCLUDE})

	# Create either an executable or library.
	if("${type}" MATCHES "^executable$")
		add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_all})
		if(DEFINED EMSCRIPTEN)
			set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--shell-file ${ROOT_DIR}/html5/shell/${PROJECT_NAME}.html")
		endif(DEFINED EMSCRIPTEN)
	elseif("${type}" MATCHES "^library$")
		add_library(${PROJECT_NAME} ${${PROJECT_NAME}_all})
	else("${type}" MATCHES "^executable$")
		message(FATAL_ERROR "Unknown project type definition for ${PROJECT_NAME}")
	endif("${type}" MATCHES "^executable$")
	set_target_properties(${PROJECT_NAME} PROPERTIES include_path "${CMAKE_CURRENT_SOURCE_DIR}")
	if(NOT ${OUTPUT_EXT} STREQUAL "")
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "${OUTPUT_EXT}")
	endif(NOT ${OUTPUT_EXT} STREQUAL "")
endfunction(dt_project type)

# Define a function which will place the current project
# in a specified folder.
function(dt_folder foldername)
	message("    .. placing in folder ${foldername}")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${foldername})
endfunction(dt_folder foldername)

# Define a function which specifies a dependency on
# the project.
function(dt_needs dependency)
	get_target_property(${dependency}_include_path ${dependency} include_path)
	if("${${dependency}_include_path}" MATCHES "NOTFOUND")
		# This is potentially a prebuilt library (like libtcod
		# which varies).
		dt_needs_prebuilt(${dependency})
	else("${${dependency}_include_path}" MATCHES "NOTFOUND")
		# We've found this as an internal dependency.
		string(LENGTH ${ROOT_DIR} root_dir_length)
		math(EXPR root_dir_length "${root_dir_length} + 1")
		string(SUBSTRING "${${dependency}_include_path}" "${root_dir_length}" -1 pretty_include_path)
		message("    .. adding '${dependency}' include at ${pretty_include_path}")
		add_dependencies(${PROJECT_NAME} ${dependency})
		include_directories("${${dependency}_include_path}")
		target_link_libraries(${PROJECT_NAME} ${dependency})
	endif("${${dependency}_include_path}" MATCHES "NOTFOUND")
endfunction(dt_needs)

# Define a function which specifies a framework/library to be linked with the project for Mac
function(dt_needs_osx_dependency dependency)
	if(APPLE)
		message("    .. adding '${dependency}' for Mac")
		find_library(${dependency}_LIBRARY ${dependency})

		# Fix for Mac libraries appending ;-framework X at the end specifically for SDL
		# if this proves to be an issue for other libraries, this case statement can be removed/expanded
		# STREQUAL is used to match SDL and not SDLmain
		if("${dependency}" STREQUAL "SDL")
			foreach(lib ${SDL_LIBRARY})
				mark_as_advanced(${lib})
				target_link_libraries(${PROJECT_NAME} "${lib}")
			endforeach(lib)
			find_path(SDL_INCLUDE_DIR SDL/SDL.h)
		else()
			mark_as_advanced(${dependency}_LIBRARY)
			target_link_libraries(${PROJECT_NAME} "${${dependency}_LIBRARY}")
		endif()
	endif(APPLE)
endfunction(dt_needs_osx_dependency dependency)

# Define a function which specifies a **pre-built**
# dependency on the project.
function(dt_needs_prebuilt dependency)
	set(should_include_headers true)
	
	# Check to see if we should exclude distributed headers on this platform.
	if(EMSCRIPTEN)
		if(DEFINED ${dependency}_EmscriptenNoInclude)
			if(${${dependency}_EmscriptenNoInclude})
				set(should_include_headers false)
			endif(${${dependency}_EmscriptenNoInclude})
		endif(DEFINED ${dependency}_EmscriptenNoInclude)
	endif(EMSCRIPTEN)
	if(UNIX)
		if(DEFINED ${dependency}_UnixNoInclude)
			if(${${dependency}_UnixNoInclude})
				set(should_include_headers false)
			endif(${${dependency}_UnixNoInclude})
		endif(DEFINED ${dependency}_UnixNoInclude)
	endif(UNIX)
	if(TOOLCHAIN_ON_CYGWIN)
		if(DEFINED ${dependency}_CygwinNoInclude)
			if(${${dependency}_CygwinNoInclude})
				set(should_include_headers false)
			endif(${${dependency}_CygwinNoInclude})
		endif(DEFINED ${dependency}_CygwinNoInclude)
	endif(TOOLCHAIN_ON_CYGWIN)
	if(${should_include_headers})
		message("    .. adding '${dependency}' include at third-party/${dependency}/include")
		include_directories("${ROOT_DIR}/third-party/${dependency}/include")
	else(${should_include_headers})
		message("    .. skipping inclusion of distributed '${dependency}' headers on this platform.")
	endif(${should_include_headers})
	
	# Add include and library dependencies.
	if(TOOLCHAIN_ON_CYGWIN AND DEFINED ${dependency}_Cygwin)
		if(DEFINED ${dependency}_Cygwin)
			message("    .. adding '${dependency}' library")
			find_library(${${dependency}_Cygwin} ${${dependency}_Cygwin})
			target_link_libraries(${PROJECT_NAME} ${${dependency}_Cygwin})
		else(DEFINED ${dependency}_Cygwin)
			message(FATAL_ERROR "    .. dependency '${dependency}' could not be satisified on your platform")
		endif(DEFINED ${dependency}_Cygwin)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		if(DEFINED ${dependency}_Windows)
			message("    .. adding '${dependency}' library at third-party/${dependency}/lib/${${dependency}_Windows}.lib")
			target_link_libraries(${PROJECT_NAME} "${ROOT_DIR}/third-party/${dependency}/lib/${${dependency}_Windows}.lib")
		else(DEFINED ${dependency}_Windows)
			message(FATAL_ERROR "    .. dependency '${dependency}' could not be satisified on your platform")
		endif(DEFINED ${dependency}_Windows)
	elseif(EMSCRIPTEN AND DEFINED ${dependency}_Emscripten)
		if(DEFINED ${dependency}_Emscripten)
			message("    .. adding '${dependency}' library at third-party/${dependency}/emscripten/build/${${dependency}_Emscripten}.js")
			target_link_libraries(${PROJECT_NAME} "${ROOT_DIR}/third-party/${dependency}/emscripten/build/${${dependency}_Emscripten}.js")
		else(DEFINED ${dependency}_Emscripten)
			message(FATAL_ERROR "    .. dependency '${dependency}' could not be satisified on your platform")
		endif(DEFINED ${dependency}_Emscripten)
	else(TOOLCHAIN_ON_CYGWIN AND DEFINED ${dependency}_Cygwin)
		if(DEFINED ${dependency}_Unix)
			message("    .. adding '${dependency}' library")
			find_library(${${dependency}_Unix} ${${dependency}_Unix})
			target_link_libraries(${PROJECT_NAME} ${${dependency}_Unix})
		else(DEFINED ${dependency}_Unix)
			message(FATAL_ERROR "    .. dependency '${dependency}' could not be satisified on your platform")
		endif(DEFINED ${dependency}_Unix)
	endif(TOOLCHAIN_ON_CYGWIN AND DEFINED ${dependency}_Cygwin)
endfunction(dt_needs_prebuilt dependency)

# Define a dependency on an executable project.
function(dt_needs_exe dependency)
	add_dependencies(${PROJECT_NAME} ${dependency})
endfunction(dt_needs_exe dependency)

# Define a function which compiles Flex and Bison files.
function(dt_has_parser cxx)
	if("${cxx}" MATCHES true)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/lexer.l
			COMMAND ${FLEX_EXECUTABLE} -o${${PROJECT_NAME}_SOURCE_DIR}/lexer.cpp --header-file=${${PROJECT_NAME}_SOURCE_DIR}/lexer.hpp ${${PROJECT_NAME}_SOURCE_DIR}/lexer.l
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/lexer.cpp ${${PROJECT_NAME}_SOURCE_DIR}/lexer.hpp)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			COMMAND ${BISON_EXECUTABLE} -d -y -o${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp ${${PROJECT_NAME}_SOURCE_DIR}/parser.hpp)
		set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp ${${PROJECT_NAME}_SOURCE_DIR}/lexer.cpp PARENT_SCOPE)
		set_source_files_properties(${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp GENERATED)
	else("${cxx}" MATCHES true)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/lexer.l
			COMMAND ${FLEX_EXECUTABLE} -o${${PROJECT_NAME}_SOURCE_DIR}/lexer.c --header-file=${${PROJECT_NAME}_SOURCE_DIR}/lexer.h ${${PROJECT_NAME}_SOURCE_DIR}/lexer.l
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/lexer.c ${${PROJECT_NAME}_SOURCE_DIR}/lexer.h)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			COMMAND ${BISON_EXECUTABLE} -d -y -o${${PROJECT_NAME}_SOURCE_DIR}/parser.c ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/parser.c ${${PROJECT_NAME}_SOURCE_DIR}/parser.h)
		set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_SOURCE_DIR}/parser.c ${${PROJECT_NAME}_SOURCE_DIR}/lexer.c PARENT_SCOPE)
		set_source_files_properties(${${PROJECT_NAME}_SOURCE_DIR}/parser.c GENERATED)
	endif("${cxx}" MATCHES true)
endfunction(dt_has_parser)

message("-- Defining projects")

# Add library folders.
if(NOT MSVC AND NOT TOOLCHAIN_ON_CYGWIN)
	add_subdirectory(third-party/opengl)
	add_subdirectory(third-party/zlib)
	add_subdirectory(third-party/libpng)
	add_subdirectory(third-party/sdl)
endif(NOT MSVC AND NOT TOOLCHAIN_ON_CYGWIN)
add_subdirectory(third-party/libtcod)
add_subdirectory(third-party/pthread)
add_subdirectory(third-party/readline)
add_subdirectory(third-party/argtable2)
add_subdirectory(third-party/bstring)
add_subdirectory(third-party/simclist)
add_subdirectory(third-party/curl)
add_subdirectory(third-party/lua)
add_subdirectory(libdcpu)
add_subdirectory(libdcpu-pp-expr)
add_subdirectory(libdcpu-vm)
add_subdirectory(libdcpu-ci-dbgfmt)
add_subdirectory(libdcpu-vm-dbg)
add_subdirectory(libdcpu-pp)
add_subdirectory(libdcpu-ci-objfmt)
add_subdirectory(libdcpu-ci-lang)
add_subdirectory(libdcpu-ci-lang-c)

# Add executable folders.
add_subdirectory(dtpp)
add_subdirectory(dtcc)
add_subdirectory(dtasm)
add_subdirectory(dtld)
add_subdirectory(dtimg)
if(${BUILD_MODULE_MANAGER})
	add_subdirectory(dtmm)
endif(${BUILD_MODULE_MANAGER})
if(${BUILD_EMULATOR})
	add_subdirectory(dtdb)
	add_subdirectory(dtemu)
endif(${BUILD_EMULATOR})

# Add documentation.
if(${BUILD_DOCUMENTATION})
	add_subdirectory(docs)
endif(${BUILD_DOCUMENTATION})

# Add scripts.
add_subdirectory(scripts)

# Add kernel and standard libraries.
if(${BUILD_KERNEL})
	if(EXISTS "${ROOT_DIR}/kernel/CMakeLists.txt")
		add_subdirectory(kernel)
	endif(EXISTS "${ROOT_DIR}/kernel/CMakeLists.txt")
endif(${BUILD_KERNEL})
if(${BUILD_STDLIB})
	add_subdirectory(stdlib-c)
endif(${BUILD_STDLIB})

# Add test suite.
enable_testing()
add_subdirectory(tests/drivers/ltestdcpu)
add_subdirectory(tests/drivers/testasm)
add_subdirectory(tests/drivers/testld)
add_subdirectory(tests/drivers/testvm)
