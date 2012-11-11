cmake_minimum_required(VERSION 2.8.5)
project(libdcpu)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.h ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.c
    COMMAND tool-errgen ${CMAKE_CURRENT_SOURCE_DIR}/err.d ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.h ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.c
    COMMENT "Generating error definition files..."
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/err.d 
    )
include_directories(
    ${bstring_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    # Need to manual path this since we have a cyclic dependency.
    ${toolchain_SOURCE_DIR}/libdcpu-pp-expr
    ${CMAKE_CURRENT_BINARY_DIR}
)
add_library(libdcpu ${LIBRARY_FORMAT}
    derr.defs.c
    derr.defs.h
    derr.c
    derr.h
    err.d
    bfile.h
    dcpu.h
    dirent.portable.h
    imap.h
    luaglob.h
    posix.h
    version.h
    bstring.h
    debug.h
    iio.h
    lexfix.h
    osutil.h
    rparse.h
    bfile.c
    bstring.c
    debug.c
    iio.c
    imap.c
    osutil.c
    rparse.c
    version.c
    config.h.in
)
cotire(libdcpu)
set_target_properties(libdcpu PROPERTIES PREFIX "")

if(DEFINED VERSION_STAGE)
    set(DCPU_CONFIG_HAS_VERSION_STAGE TRUE)
    set(DCPU_CONFIG_VERSION_STAGE ${VERSION_STAGE})
endif()
if(DEFINED VERSION_GIT)
    set(DCPU_CONFIG_HAS_VERSION_GIT TRUE)
    set(DCPU_CONFIG_VERSION_GIT ${VERSION_GIT})
endif()
set(KERNEL_DEFAULT "stubsys" CACHE STRING "The name of the default kernel that the toolchain should use for linking")
if(${DEPLOYABLE})
    if(DEFINED KERNEL_DEFAULT)
        set(DCPU_CONFIG_HAS_KERNEL_DEFAULT TRUE)
        set(DCPU_CONFIG_KERNEL_DEFAULT ${KERNEL_DEFAULT})
    endif()
    set(DCPU_CONFIG_HAS_MODULE_PATH TRUE)
    set(DCPU_CONFIG_HAS_KERNEL_PATH TRUE)
    set(DCPU_CONFIG_HAS_STDLIB_PATH TRUE)
    set(DCPU_CONFIG_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/dcpu/modules)
    set(DCPU_CONFIG_KERNEL_PATH ${CMAKE_INSTALL_PREFIX}/share/dcpu/kernels)
    set(DCPU_CONFIG_STDLIB_PATH ${CMAKE_INSTALL_PREFIX}/share/dcpu/stdlib)
endif()
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
)

if(${DEPLOYABLE})
    target_link_libraries(libdcpu
        bstring
    )
    install(TARGETS libdcpu
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers_gen "${CMAKE_CURRENT_BINARY_DIR}/*.h")
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} ${headers_gen} DESTINATION include/dcpu)
endif()
