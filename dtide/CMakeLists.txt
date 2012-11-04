cmake_minimum_required(VERSION 2.8)
project(dtide)

find_package(OpenGL REQUIRED)
find_package(Qt4)
if(${QT4_FOUND} AND ${qscintilla2_FOUND})
    set(dtide_SOURCES 
        main.cpp 
        Backends.cpp
        DTIDE.cpp
        DTIDETabWidget.cpp
        DTIDESplash.cpp
        DTIDEBuildAPI.cpp
        DTIDEHighlighting.cpp
        DTIDEDebuggingSession.cpp
        DTIDEDebuggingWindow.cpp
        qhexview/qhexview.cpp
        DTIDEGLWidgets.cpp
        CodeEditor.cpp
        Highlighters/QsciLexerDASM16.cpp
        Toolchains/DT/Toolchain.cpp
        Toolchains/DT/dtasm.c
        Toolchains/DT/dtemu.c
        Toolchains/DT/dtdb.c
    )
    set(dtide_HEADERS
        Backends.h
        DTIDE.h
        DTIDETabWidget.h
        DTIDESplash.h
        DTIDEBuildAPI.h
        DTIDEHighlighting.h
        DTIDEDebuggingSession.h
        DTIDEDebuggingWindow.h
        qhexview/qhexview.h
        DTIDEGLWidgets.h
        CodeEditor.h
        Highlighters/Highlighters.h
        Highlighters/QsciLexerDASM16.h
        Toolchains/Toolchains.h
        Toolchains/DT/Toolchain.h
        Toolchains/DT/dtasm.h
        Toolchains/DT/dtemu.h
        Toolchains/DT/dtdb.h
    )
        
    set(dtide_FORMS 
        "ui/splash.ui"
        "ui/registers.ui"
    )

    QT4_WRAP_CPP(dtide_HEADERS_MOC ${dtide_HEADERS})
    QT4_WRAP_UI(dtide_FORMS_HEADERS ${dtide_FORMS})

    set(QT_USE_QTOPENGL TRUE)
    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})

    include_directories(
        ${dtide_SOURCE_DIR}
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
        ${libdcpu-vm_SOURCE_DIR}
        ${libdcpu-vm-dbg_SOURCE_DIR}
        ${dtide_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${GLFW_SOURCE_DIR}/include
        ${png_SOURCE_DIR}
        ${qscintilla2_SOURCE_DIR} 
    )

    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()

    if(MSVC)
        add_definitions(-DQSCINTILLA_DLL)
    endif()

    add_executable(dtide ${dtide_SOURCES}
        ${dtide_HEADERS_MOC}
        ${dtide_HEADERS}
        ${dtide_FORMS_HEADERS}
    )
    target_link_libraries(dtide
        ${QT_LIBRARIES}
        bstring
        argtable2
        libdcpu-ci-asm
        libdcpu-vm
        libdcpu-ci-dbgfmt
        libdcpu-vm-dbg
        png
        z
        glfw
        simclist
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${qscintilla2_LIBRARY}
    )

    if(${DEPLOYABLE})
        install(TARGETS dtide
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)
    endif()
else()
    if(NOT ${QT_FOUND})
        message("Not building IDE due to lack of Qt 4 libraries.")
    else()
        message("Not building IDE due to lack of Qscintilla libraries.")
    endif()
endif()

