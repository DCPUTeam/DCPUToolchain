cmake_minimum_required (VERSION 2.6)
project (dcputoolchain)

SET(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ROOT_DIR}/emulator)

add_subdirectory(compiler)
add_subdirectory(emulator)
add_subdirectory(assembler)
