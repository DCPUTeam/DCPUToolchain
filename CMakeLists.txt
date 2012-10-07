cmake_minimum_required(VERSION 2.8.5)
project(toolchain)

# Main version information.
set(VERSION_STAGE "beta")
execute_process(COMMAND git rev-parse HEAD WORKING_DIRECTORY ${toolchain_SOURCE_DIR} OUTPUT_VARIABLE git_hash)
if("${git_hash}" STREQUAL "")
	add_definitions(-DVERSION_STAGE=${VERSION_STAGE})
else("${git_hash}" STREQUAL "")
	string(STRIP ${git_hash} git_hash)
	string(SUBSTRING ${git_hash} 0 9 git_hash)
	set(VERSION_GIT ${git_hash})
	add_definitions(-DVERSION_STAGE=${VERSION_STAGE} -DVERSION_GIT=${VERSION_GIT})
endif("${git_hash}" STREQUAL "")

# Set up some basics.
set(CMAKE_MODULE_PATH "${toolchain_SOURCE_DIR}/CMakeScripts")
set(FLEX_UNISTD_INCLUDE "")
set(CMAKE_BUILD_TYPE Debug)
if(MSVC)
	# Windows does not have unistd.h, but GnuWin32's Flex program
	# provides it for us, so we need to add that as an include
	# directory in this case.
	set(FLEX_UNISTD_INCLUDE "${toolchain_SOURCE_DIR}/third-party/flex")
endif()
if(NOT DEFINED FLEX_EXECUTABLE)
	find_program(FLEX_EXECUTABLE NAMES flex.exe flex)
endif()
if(NOT DEFINED BISON_EXECUTABLE)
	find_program(BISON_EXECUTABLE NAMES bison.exe bison)
endif()
if(NOT DEFINED M4_EXECUTABLE)
	find_program(M4_EXECUTABLE NAMES m4.exe m4)
endif()

# Set compiler options.
get_filename_component(CMAKE_C_COMPILER_BASENAME ${CMAKE_C_COMPILER} NAME_WE)
set(OUTPUT_EXT "")
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
	# This is to deal with a bug introduced in the MSVC linker when both VS2010 and VS2012 are
	# installed on the same machine.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
elseif(APPLE)
	# TODO: Set warnings as errors under Mac / XCode.
elseif(MINGW)
	# This is a cross of other stuff in here; configured for building under unix, for windows :)
	# This method of compilation is extrememly advised against; it could work, but you are better off getting official
	# binaries for windows, ones that have been tested.
	# A major reason for this is because the -Werror flag has been removed.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement -Wno-int-to-pointer-cast -D SIMCLIST_NO_DUMPRESTORE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D SIMCLIST_NO_DUMPRESTORE")
elseif(${CMAKE_C_COMPILER_BASENAME} STREQUAL "clang" OR ${CMAKE_C_COMPILER_BASENAME} STREQUAL "emcc")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu89 -Werror -Wdeclaration-after-statement -Wno-unused-value -Wno-pointer-sign -Wno-int-to-pointer-cast -Wno-switch -Qunused-arguments")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# Show configuration.
message("-- Showing configuration")
if(DEFINED VERSION_GIT)
	message("  >> version is: '${VERSION_STAGE}; #${VERSION_GIT}'")
else()
	message("  >> version is: '${VERSION_STAGE}'")
endif()
message("  >> m4 is at: ${M4_EXECUTABLE}")
message("  >> flex is at: ${FLEX_EXECUTABLE}")
message("  >> flex unistd is at: ${FLEX_UNISTD_INCLUDE}")
message("  >> bison is at: ${BISON_EXECUTABLE}")
message("   (must be Cygwin versions on Windows)")

# Add library folders.
#if(NOT MSVC AND NOT TOOLCHAIN_ON_CYGWIN)
#	add_subdirectory(third-party/opengl)
#	add_subdirectory(third-party/zlib)
#	add_subdirectory(third-party/libpng)
#	add_subdirectory(third-party/sdl)
#endif(NOT MSVC AND NOT TOOLCHAIN_ON_CYGWIN)
#add_subdirectory(third-party/glfw)
#add_subdirectory(third-party/libtcod)
#add_subdirectory(third-party/pthread)
#add_subdirectory(third-party/readline)
#add_subdirectory(third-party/argtable2)
add_subdirectory(third-party/bstring)
#add_subdirectory(third-party/simclist)
#add_subdirectory(third-party/curl)
#add_subdirectory(third-party/lua)
add_subdirectory(libdcpu)
#add_subdirectory(libdcpu-pp-expr)
#add_subdirectory(libdcpu-vm)
#add_subdirectory(libdcpu-ci-dbgfmt)
#add_subdirectory(libdcpu-vm-dbg)
#add_subdirectory(libdcpu-pp)
#add_subdirectory(libdcpu-ci-objfmt)
#add_subdirectory(libdcpu-ci-lang)
#add_subdirectory(libdcpu-ci-lang-c)
#add_subdirectory(libdcpu-vm-python)

# Add executable folders.
#add_subdirectory(dtpp)
#add_subdirectory(dtcc)
#add_subdirectory(dtasm)
#add_subdirectory(dtld)
#add_subdirectory(dtimg)
#add_subdirectory(dtlua)
#add_subdirectory(dtmm)
#add_subdirectory(dtdb)
#add_subdirectory(dtemu)

# Add documentation.
#add_subdirectory(docs)

# Add scripts.
#add_subdirectory(scripts)

# Add kernel and standard libraries.
#if(EXISTS "${ROOT_DIR}/kernel/CMakeLists.txt")
#	add_subdirectory(kernel)
#endif(EXISTS "${ROOT_DIR}/kernel/CMakeLists.txt")
#add_subdirectory(stdlib-c)

# Add test suite.
#enable_testing()
#add_subdirectory(tests/drivers/ltestdcpu)
#add_subdirectory(tests/drivers/testasm)
#add_subdirectory(tests/drivers/testld)
#add_subdirectory(tests/drivers/testvm)
