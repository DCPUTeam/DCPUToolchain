cmake_minimum_required(VERSION 2.8.5)
project(curl)

add_library(curl SHARED IMPORTED)
set_property(TARGET curl PROPERTY FOLDER "third-party")
set_property(TARGET curl PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/libcurl.dll")
