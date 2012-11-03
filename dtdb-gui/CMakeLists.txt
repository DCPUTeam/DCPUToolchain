cmake_minimum_required(VERSION 2.8)
project(dtdb-gui)

find_package(Qt4)
if(${QT4_FOUND})
    find_package(OpenGL REQUIRED)

    set(dtdb-gui_SOURCES 
        main.cpp 
        mainwindow.cpp
        dtdbrunner.cpp)
    set(dtdb-gui_HEADERS 
        mainwindow.h
        dtdbrunner.h)
        
    set(dtdb-gui_FORMS dtdb.ui)

    QT4_WRAP_CPP(dtdb-gui_HEADERS_MOC ${dtdb-gui_HEADERS})
    QT4_WRAP_UI(dtdb-gui_FORMS_HEADERS ${dtdb-gui_FORMS})

    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})

    include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${readline_SOURCE_DIR}
        ${pthread_SOURCE_DIR}
        ${argtable2_SOURCE_DIR}
        ${simclist_SOURCE_DIR}
        ${bstring_SOURCE_DIR}
        ${lua_SOURCE_DIR}
        ${libdcpu-pp-expr_SOURCE_DIR}
        ${libdcpu-ci-dbgfmt_SOURCE_DIR}
        ${libdcpu-vm-dbg_SOURCE_DIR}
        ${libdcpu-vm-dbg_BINARY_DIR}
        ${libdcpu-vm_SOURCE_DIR}
        ${libdcpu_SOURCE_DIR}
        ${FLEX_UNISTD_INCLUDE}
        ${GLFW_SOURCE_DIR}/include
        ${png_SOURCE_DIR}
    )
    add_executable(dtdb-gui ${dtdb-gui_SOURCES}
        ${dtdb-gui_HEADERS_MOC}
        ${dtdb-gui_FORMS_HEADERS}
    )
    target_link_libraries(dtdb-gui
        ${QT_LIBRARIES}
        ${readline_LIBRARY}
        ${pthread_LIBRARY}
        argtable2
        simclist
        lua
        libdcpu-pp-expr
        libdcpu-ci-dbgfmt
        libdcpu-vm-dbg
        libdcpu-vm
        libdcpu
        bstring
        glfw
        png
        z
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
    )

else()
    message("Not building GUI debugger due to lack of Qt 4 libraries.")
endif()

