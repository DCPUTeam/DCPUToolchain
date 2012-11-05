cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm)

add_executable(embedfont embedfont.c)
include_directories(${libdcpu_SOURCE_DIR})
target_link_libraries(embedfont libdcpu)
if(MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/font.c
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/embedfont${CMAKE_EXECUTABLE_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/font.c ${CMAKE_CURRENT_SOURCE_DIR}/font.png
        DEPENDS font.png embedfont)
else()
     add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/font.c
        COMMAND embedfont ${CMAKE_CURRENT_BINARY_DIR}/font.c ${CMAKE_CURRENT_SOURCE_DIR}/font.png
        DEPENDS font.png embedfont)
endif()

find_package(OpenGL REQUIRED)
add_library(libdcpu-vm ${LIBRARY_FORMAT}
    dcpu.c
    dcpubase.h
    dcpudis.h
    dcpuhook.h
    dcpuops.h
    glfwutils.h
    hw.h
    hwioascii.h
    hwkeyboard.h
    hwlem1802.h
    hwlem1802mem.h
    hwlem1802util.h
    hwlua.h
    hwluacpu.h
    hwm35fd.h
    hwsped3.h
    hwtimer.h
    timing.h
    dcpubase.c
    dcpudis.c
    dcpuhook.c
    dcpuops.c
    glfwutils.c
    hw.c
    hwioascii.c
    hwkeyboard.c
    hwlem1802.c
    hwlem1802mem.c
    hwlem1802util.c
    hwlua.c
    hwluacpu.c
    hwm35fd.c
    hwsped3.c
    hwtimer.c
    timing.c
    ${CMAKE_CURRENT_BINARY_DIR}/font.c
)
include_directories(
    ${png_SOURCE_DIR}
    ${png_BINARY_DIR}
    ${z_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${lua_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${GLFW_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)
target_link_libraries(libdcpu-vm
    libdcpu
)
set_target_properties(libdcpu-vm PROPERTIES PREFIX "")
cotire(libdcpu-vm)

if(${DEPLOYABLE})
    target_link_libraries(libdcpu-vm
        z
        png
        glfw
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
    install(TARGETS libdcpu-vm
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} DESTINATION include/dcpu/vm)
endif()
