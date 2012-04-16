SET(EMULATOR_SRC dcpubase.c dcpu.c dcpuhook.c dcpuops.c hwioascii.c hwio.c
    main.c)

# Should pass -DLIBTCOD_DIR=/path/to/built/libtcod to cmake
include_directories(${LIBTCOD_DIR}/include)
link_directories(${LIBTCOD_DIR})

add_executable(emulator ${EMULATOR_SRC})
target_link_libraries(emulator tcod argtable2)
