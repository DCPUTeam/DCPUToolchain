cmake_minimum_required(VERSION 2.8)
project(dtide)

find_package(Qt4)
if(${QT4_FOUND})
    set(dtide_SOURCES 
        main.cpp 
        mainwindow.cpp
        highlighters/dcpuhighlighter.cpp
        toolchains/dcputoolchain.cpp
        toolchains/dcputoolchain_asm.c
        dtidetabwidget.cpp
        dtidesplash.cpp
        dtidehighlighting.cpp
        dtidebuildapi.cpp
        backends.cpp
        codeeditor.cpp)
    set(dtide_HEADERS 
        mainwindow.h
        highlighters/dcpuhighlighter.h
        dtidetabwidget.h
        dtidesplash.h
        codeeditor.h)
        
    set(dtide_FORMS "ui/splash.ui")

    QT4_WRAP_CPP(dtide_HEADERS_MOC ${dtide_HEADERS})
    QT4_WRAP_UI(dtide_FORMS_HEADERS ${dtide_FORMS})

    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})

    include_directories(
        ${simclist_SOURCE_DIR}
        ${argtable2_SOURCE_DIR}
        ${bstring_SOURCE_DIR}
        ${libdcpu_SOURCE_DIR}
        ${libdcpu-ci-asm_SOURCE_DIR}
        ${libdcpu-ci-objfmt_SOURCE_DIR}
        ${libdcpu-pp_SOURCE_DIR}
        ${libdcpu-pp-expr_SOURCE_DIR}
        ${libdcpu-ci-objfmt_SOURCE_DIR}
        ${libdcpu-ci-dbgfmt_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

    add_executable(dtide ${dtide_SOURCES}
        ${dtide_HEADERS_MOC}
        ${dtide_FORMS_HEADERS}
    )
    target_link_libraries(dtide
        ${QT_LIBRARIES}
        bstring
        argtable2
        libdcpu-ci-asm
    )
else()
    message("Not building IDE due to lack of Qt 4 libraries.")
endif()

