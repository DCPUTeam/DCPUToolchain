cmake_minimum_required(VERSION 2.8)
project(dtide)

find_package(Qt4)
if(${QT4_FOUND})
    set(dtide_SOURCES 
        main.cpp 
        mainwindow.cpp
        dcpuhighlighter.cpp
        dtidetabwidget.cpp
        codeeditor.cpp)
    set(dtide_HEADERS 
        mainwindow.h
        dcpuhighlighter.h
        dtidetabwidget.h
        codeeditor.h)
        
    set(dtide_FORMS "")

    QT4_WRAP_CPP(dtide_HEADERS_MOC ${dtide_HEADERS})
    QT4_WRAP_UI(dtide_FORMS_HEADERS ${dtide_FORMS})

    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})

    include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_executable(dtide ${dtide_SOURCES}
        ${dtide_HEADERS_MOC}
        ${dtide_FORMS_HEADERS}
    )
    target_link_libraries(dtide
        ${QT_LIBRARIES}
    )
else()
    message("Not building GUI debugger due to lack of Qt 4 libraries.")
endif()

