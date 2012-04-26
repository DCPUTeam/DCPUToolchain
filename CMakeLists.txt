cmake_minimum_required(VERSION 2.6)
project (toolchain)

# Set up some basics.
set(IS_ROOT 1)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR})
else(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_DIR}/Debug")
endif(MSVC)
set(OUTPUT_DIR "${ROOT_DIR}/Debug")
set(FLEX_UNISTD_INCLUDE "")
if(MSVC)
	# Windows does not have unistd.h, but GnuWin32's Flex program
	# provides it for us, so we need to add that as an include
	# directory in this case.
	set(FLEX_UNISTD_INCLUDE "C:/UNIX/GnuWin32/include")
endif(MSVC)
if(NOT DEFINED BUILD_EMULATOR)
	set(BUILD_EMULATOR true)
endif(NOT DEFINED BUILD_EMULATOR)
if(NOT DEFINED FLEX_EXECUTABLE)
	find_program(FLEX_EXECUTABLE NAMES flex.exe flex)
endif(NOT DEFINED FLEX_EXECUTABLE)
if(NOT DEFINED BISON_EXECUTABLE)
	find_program(BISON_EXECUTABLE NAMES bison.exe bison)
endif(NOT DEFINED BISON_EXECUTABLE)
if(NOT DEFINED M4_EXECUTABLE)
	find_program(M4_EXECUTABLE NAMES m4.exe m4)
endif(NOT DEFINED M4_EXECUTABLE)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

message("-- Showing configuration")
message("  >> m4 is at: ${M4_EXECUTABLE}")
message("  >> flex is at: ${FLEX_EXECUTABLE}")
message("  >> flex unistd is at: ${FLEX_UNISTD_INCLUDE}")
message("  >> bison is at: ${BISON_EXECUTABLE}")
message("   (must be GnuWin32 versions on Windows; Cygwin does not work)")

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
	elseif("${type}" MATCHES "^library$")
		add_library(${PROJECT_NAME} ${${PROJECT_NAME}_all})
	else("${type}" MATCHES "^executable$")
		message(FATAL_ERROR "Unknown project type definition for ${PROJECT_NAME}")
	endif("${type}" MATCHES "^executable$")
	set_property(TARGET ${PROJECT_NAME} PROPERTY include_path "${CMAKE_CURRENT_SOURCE_DIR}")
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
	#if(NOT ${${dependency}_include_path})
	#	message(FATAL_ERROR "  .. unable to find include path of '${dependency}'")
	#endif(NOT ${${dependency}_include_path})
	string(LENGTH ${ROOT_DIR} root_dir_length)
	math(EXPR root_dir_length "${root_dir_length} + 1")
	string(SUBSTRING "${${dependency}_include_path}" "${root_dir_length}" -1 pretty_include_path)
	message("    .. adding '${dependency}' include at ${pretty_include_path}")
	add_dependencies(${PROJECT_NAME} ${dependency})
	include_directories("${${dependency}_include_path}")
	target_link_libraries(${PROJECT_NAME} ${dependency})
endfunction(dt_needs)

# Define a function which specifies a **pre-built**
# dependency on the project.
function(dt_needs_prebuilt dependency libname)
	message("    .. adding '${dependency}' library at third-party/${dependency}/lib/${libname}.lib")
	message("    .. adding '${dependency}' include at third-party/${dependency}/include")
	include_directories("${ROOT_DIR}/third-party/${dependency}/include")
	target_link_libraries(${PROJECT_NAME} "${ROOT_DIR}/third-party/${dependency}/lib/${libname}.lib")
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
			COMMAND ${FLEX_EXECUTABLE} -olexer.cpp lexer.l
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/lexer.cpp)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			COMMAND ${BISON_EXECUTABLE} -d -y -oparser.cpp parser.y
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp ${${PROJECT_NAME}_SOURCE_DIR}/parser.hpp)
		set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp ${${PROJECT_NAME}_SOURCE_DIR}/lexer.cpp PARENT_SCOPE)
		set_source_files_properties(${${PROJECT_NAME}_SOURCE_DIR}/parser.cpp GENERATED)
	else("${cxx}" MATCHES true)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/lexer.l
			COMMAND ${FLEX_EXECUTABLE} -olexer.c lexer.l
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/lexer.c)
		add_custom_command(
			SOURCE ${${PROJECT_NAME}_SOURCE_DIR}/parser.y
			COMMAND ${BISON_EXECUTABLE} -d -y -oparser.c parser.y
			TARGET ${PROJECT_NAME}
			OUTPUTS ${${PROJECT_NAME}_SOURCE_DIR}/parser.c ${${PROJECT_NAME}_SOURCE_DIR}/parser.h)
		set(${PROJECT_NAME}_all ${${PROJECT_NAME}_all} ${${PROJECT_NAME}_SOURCE_DIR}/parser.c ${${PROJECT_NAME}_SOURCE_DIR}/lexer.c PARENT_SCOPE)
		set_source_files_properties(${${PROJECT_NAME}_SOURCE_DIR}/parser.c GENERATED)
	endif("${cxx}" MATCHES true)
endfunction(dt_has_parser)

message("-- Defining projects")

# Add library folders.
add_subdirectory(third-party/libtcod)
add_subdirectory(third-party/argtable2)
add_subdirectory(third-party/bstring)
add_subdirectory(third-party/simclist)
add_subdirectory(libdcpu)
add_subdirectory(libdcpu-vm)
add_subdirectory(libdcpu-pp)
add_subdirectory(libdcpu-ci-objfmt)
add_subdirectory(libdcpu-ci-lang)
add_subdirectory(libdcpu-ci-lang-c)

# Add executable folders.
add_subdirectory(preprocessor)
add_subdirectory(compiler)
add_subdirectory(assembler)
add_subdirectory(linker)
if("${BUILD_EMULATOR}" MATCHES true)
	add_subdirectory(emulator)
endif("${BUILD_EMULATOR}" MATCHES true)

# Add kernel, using the multi-threading version if available.
if(EXISTS "kernel.mt" AND IS_DIRECTORY "kernel.mt")
	add_subdirectory(kernel.mt)
else(EXISTS "kernel.mt" AND IS_DIRECTORY "kernel.mt")
	add_subdirectory(kernel)
endif(EXISTS "kernel.mt" AND IS_DIRECTORY "kernel.mt")
