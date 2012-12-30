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
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
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

option(USE_COTIRE "Use cotire to speed up builds" ON)

# Configure options relating to standalone or deployed
# configuration.
option(DEPLOYABLE "Build a deployable version of the toolchain" OFF)
if(${DEPLOYABLE})
    message("===== Deployable Build Activated =====")
    message("Deployable builds should be used when you intend")
    message("to install a final set of binaries on a system that")
    message("supports dynamic linking and package management.")
    message("")
    message("Deployable builds will also install development")
    message("headers for external use.")
    message("======================================")
    if(MSVC)
        # MSVC has explicit exporting with dllexport, but we're not going to
        # add that to every single function declaration.  On Windows, we always
        # static link.
        set(LIBRARY_FORMAT STATIC)
        set(CMAKE_INSTALL_PREFIX "C:/DCPU")
    else()
        set(LIBRARY_FORMAT SHARED)
    endif()
else()
    set(LIBRARY_FORMAT STATIC)
endif()

# Set compiler options.
set(OUTPUT_EXT "")
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /wd4273 /wd4065 /wd4996 /wd4005 /D_CRT_SECURE_NO_WARNINGS /DSIMCLIST_NO_DUMPRESTORE")
    option(VS2010_VS2012_CONFLICT_WORKAROUND "Fixes linking issues when building on a computer with both VS2010 and VS2012 installed." OFF)
    if(${VS2010_VS2012_CONFLICT_WORKAROUND})
        # This is to deal with a bug introduced in the MSVC linker when both VS2010 and VS2012 are
        # installed on the same machine.
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
    endif()
elseif(APPLE)
    # TODO: Set warnings as errors under Mac / XCode.
elseif(MINGW)
    # This is a cross of other stuff in here; configured for building under unix, for windows :)
    # This method of compilation is extrememly advised against; it could work, but you are better off getting official
    # binaries for windows, ones that have been tested.
    # A major reason for this is because the -Werror flag has been removed.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement -Wno-int-to-pointer-cast -D SIMCLIST_NO_DUMPRESTORE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D SIMCLIST_NO_DUMPRESTORE")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Configuration for Clang.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-pointer-sign -Wno-builtin-requires-header")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
else(MSVC)
    # Assume GNU since there doesn't appear to be
    # a better way of detecting this.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wdeclaration-after-statement -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-value")
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

# Speed up build with cotire.
if(MSVC)
    message("  >> compiler is MSVC -> NOT using cotire")
    function(cotire proj)
    endfunction()
else()
    # use cotire only when the version is 2.8.6. or greater
    if (CMAKE_VERSION VERSION_GREATER 2.8.5)
        if(${USE_COTIRE})
            message("  >> cmake version is >= 2.8.6 -> using cotire")
            include(cotire)
        else()
            function(cotire proj)
            endfunction()
        endif()
    else()
        message("  >> cmake version is < 2.8.6 -> NOT using cotire")
        function(cotire proj)
        endfunction()
    endif()
endif()

# Define a function that automatically sorts
# source files into groups for Visual Studio.
function(auto_group project)
    if(MSVC)
        get_property(srcs TARGET ${project} PROPERTY SOURCES)
        foreach(o ${srcs})
            get_filename_component(oabs ${o} ABSOLUTE)
            file(RELATIVE_PATH i "${${project}_SOURCE_DIR}" "${oabs}")
            get_filename_component(ipath ${i} PATH)
            get_filename_component(iext ${i} EXT)
            if(NOT "${ipath}" STREQUAL "")
                string(REPLACE / \\ winpath ${ipath})
                if("${iext}" STREQUAL ".c" OR "${iext}" STREQUAL ".cpp")
                    source_group(Source\ Files\\${winpath} FILES ${o})
                elseif("${iext}" STREQUAL ".h")
                    source_group(Header\ Files\\${winpath} FILES ${o})
                endif()
            endif()
        endforeach()
    endif()
endfunction()

# Set GLFW options.
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)

# Define available build options.
option(BUILD_KERNELS "Build all the kernels" ON)
option(BUILD_STDLIB "Build the C standard library" ON)
option(BUILD_DOCS "Build the documentation if possible" ON)
option(BUILD_TOOLS "Build the actual tools" ON)
option(RUNTIME_ONLY "Only build libdcpu, libdcpu-vm and dependencies" OFF)

# Find Bison and Flex
find_package(FLEX)
find_package(BISON)

add_subdirectory(third-party/bstring)
add_subdirectory(third-party/lua)
add_subdirectory(third-party/readline)
add_subdirectory(third-party/qscintilla2)

add_subdirectory(tool-errgen)
add_subdirectory(libdcpu)
add_subdirectory(libdcpu-pp-expr)
add_subdirectory(libdcpu-vm)
if(NOT ${RUNTIME_ONLY})
    add_subdirectory(libdcpu-ci-dbgfmt)
    add_subdirectory(libdcpu-vm-dbg)
    add_subdirectory(libdcpu-pp)
    add_subdirectory(libdcpu-ci-objfmt)
    add_subdirectory(libdcpu-ci-lang)
    add_subdirectory(libdcpu-ci-lang-c)
    add_subdirectory(libdcpu-ci-asm)
    add_subdirectory(libdcpu-ld-policy)
    add_subdirectory(libdcpu-ld)
endif()

## Add executable folders.
if(${BUILD_TOOLS} AND NOT ${RUNTIME_ONLY})
    add_subdirectory(dtpp)
    add_subdirectory(dtcc)
    add_subdirectory(dtasm)
    add_subdirectory(dtld)
    add_subdirectory(dtpolicy)
    add_subdirectory(dtide)
    add_subdirectory(dtimg)
    add_subdirectory(dtlua)
    add_subdirectory(dtmm)
    add_subdirectory(dtdb)
    add_subdirectory(dtdb-gui)
    add_subdirectory(dtemu)
endif()
if(EXISTS "${toolchain_SOURCE_DIR}/dtgame/CMakeLists.txt")
    add_subdirectory(dtgame)
endif()

# Add documentation.
if(${BUILD_DOCS})
    add_subdirectory(docs)
endif()

# Add kernel and standard libraries.
if(${BUILD_KERNELS})
    add_subdirectory(kernel)
endif()
if(${BUILD_STDLIB})
    add_subdirectory(stdlib-c)
endif()

# Add test suite.
if(${BUILD_TOOLS} AND NOT ${RUNTIME_ONLY})
    enable_testing()
    add_subdirectory(tests/drivers/ltestdcpu)
    add_subdirectory(tests/drivers/ltestdcpu-pp)
    add_subdirectory(tests/drivers/testasm)
    add_subdirectory(tests/drivers/testld)
    add_subdirectory(tests/drivers/testvm)
endif()

# Additional deployment configuration.
if(${DEPLOYABLE})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/modules)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/modules
        DESTINATION share/dcpu
        DIRECTORY_PERMISSIONS
            OWNER_READ GROUP_READ WORLD_READ
            OWNER_WRITE GROUP_WRITE WORLD_WRITE
            OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules/README.md")
        file(GLOB modules "${CMAKE_CURRENT_SOURCE_DIR}/modules/*.lua")
        install(FILES
            ${modules}
            DESTINATION
            share/dcpu/modules)
    endif()

    # Additional DLLs to be installed.
    if(MSVC)
        install(FILES ${QT_BINARY_DIR}/QtCored4.dll DESTINATION bin)
        install(FILES ${QT_BINARY_DIR}/QtGuid4.dll DESTINATION bin)
        install(FILES ${QT_BINARY_DIR}/QtOpenGLd4.dll DESTINATION bin)
        install(FILES ${QT_BINARY_DIR}/QtXmld4.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/readline/bin/history5.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/readline/bin/readline5.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/curl/bin/libcurl.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/msvcp100d.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/msvcr100d.dll DESTINATION bin)
        install(FILES ${toolchain_SOURCE_DIR}/third-party/pthread/bin/pthreadVC2.dll DESTINATION bin)
    endif()

    # Configure packaging.
    if(MSVC)
        set(CPACK_GENERATOR "NSIS")
    elseif(APPLE)
        set(CPACK_GENERATOR "Bundle")
    else()
        set(CPACK_GENERATOR "RPM;DEB")
    endif()
    set(CPACK_NSIS_DISPLAY_NAME "DCPU-16 Toolchain")
    set(CPACK_PACKAGE_CONTACT "DCPU-16 Developers")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${toolchain_SOURCE_DIR}/package/DESCRIPTION.txt")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The best set of tools for developing applications on the DCPU-16.")
    set(CPACK_RESOURCE_FILE_LICENSE "${toolchain_SOURCE_DIR}/package/LICENSE.txt")
    set(CPACK_RESOURCE_FILE_README "${toolchain_SOURCE_DIR}/package/DESCRIPTION.txt")
    set(CPACK_PACKAGE_NAME "dcputoolchain")
    set(CPACK_PACKAGE_VENDOR "DCPU-16 Developers")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_GIT}")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT License")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "James Rhodes <jrhodes@redpointsoftware.com.au>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Development")

    if(MSVC)
        set(CMAKE_INSTALL_PREFIX "C:/DCPU")
        set(CPACK_PACKAGE_NAME "DCPU-16 Toolchain")
        set(CPACK_NSIS_INSTALL_ROOT "C:")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "DCPU")
        set(CPACK_NSIS_MODIFY_PATH ON)
        set(CPACK_NSIS_MUI_FINISHPAGE_RUN "dtide.exe")
        set(CPACK_PACKAGE_EXECUTABLES "dtide;Toolchain IDE")
    elseif(APPLE)
        set(CPACK_BUNDLE_NAME "DCPU-16 Toolchain")
        set(CPACK_BUNDLE_ICON "${toolchain_SOURCE_DIR}/package/MacIcon.png")
        set(CPACK_BUNDLE_PLIST "${toolchain_SOURCE_DIR}/package/Info.plist")
        set(CPACK_BUNDLE_STARTUP_COMMAND "${toolchain_SOURCE_DIR}/package/MacStart.sh")
    endif()

    include(CPack)
endif()
