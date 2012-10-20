cmake_minimum_required(VERSION 2.8)
project(dtide)

find_package(OpenGL REQUIRED)
find_package(Qt4)
if(${QT4_FOUND})
    set(dtide_SOURCES 
        main.cpp 
        Backends.cpp
        DTIDE.cpp
        DTIDETabWidget.cpp
        DTIDESplash.cpp
        DTIDEBuildAPI.cpp
        DTIDEHighlighting.cpp
        CodeEditor.cpp
        Highlighters/DCPUHighlighter.cpp
        Toolchains/DT/Toolchain.cpp
        Toolchains/DT/dtasm.c
        Toolchains/DT/dtemu.c
    )
    set(dtide_HEADERS
        Backends.h
        DTIDE.h
        DTIDETabWidget.h
        DTIDESplash.h
        DTIDEBuildAPI.h
        DTIDEHighlighting.h
        CodeEditor.h
        Highlighters/Highlighters.h
        Highlighters/DCPUHighlighter.h
        Toolchains/Toolchains.h
        Toolchains/DT/Toolchain.h
        Toolchains/DT/dtasm.h
        Toolchains/DT/dtemu.h
    )
        
    set(dtide_FORMS "UI/splash.ui")

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
        ${libdcpu-vm_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${GLFW_SOURCE_DIR}/include
        ${png_SOURCE_DIR}
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

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
        png
        z
        glfw
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
else()
    message("Not building IDE due to lack of Qt 4 libraries.")
endif()

