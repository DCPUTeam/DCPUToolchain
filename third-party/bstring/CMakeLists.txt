cmake_minimum_required(VERSION 2.8.5)
project(bstring)

add_library(bstring STATIC
    bstraux.h
    bstrlib.h
    bstrwrap.h
    bstraux.c
    bstrlib.c
)
set_property(TARGET bstring PROPERTY FOLDER "third-party")
cotire(bstring)
