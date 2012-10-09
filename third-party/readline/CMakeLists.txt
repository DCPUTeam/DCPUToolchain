cmake_minimum_required(VERSION 2.8.5)
project(readline)

add_library(readline SHARED IMPORTED)
set_property(TARGET readline PROPERTY FOLDER "third-party")
set_property(TARGET readline PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/readline5.dll")
set_property(TARGET readline PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/bin/history5.dll")

