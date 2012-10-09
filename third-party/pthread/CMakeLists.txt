cmake_minimum_required(VERSION 2.8.5)
project(pthread)

add_library(pthread SHARED IMPORTED)
set_property(TARGET pthread PROPERTY FOLDER "third-party")
set_property(TARGET pthread PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/pthreadVC2.dll")
