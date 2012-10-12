cmake_minimum_required(VERSION 2.8.5)
project(simclist)

add_library(simclist STATIC
    simclist.h
    simclist.c
)
add_definitions("-DSIMCLIST_NO_DUMPRESTORE")
set_property(TARGET simclist PROPERTY FOLDER "third-party")
cotire(simclist)
