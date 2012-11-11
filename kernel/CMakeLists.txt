cmake_minimum_required(VERSION 2.8.5)

include(dcpu.cmake)

# Define a list of kernel folders that we know about.
set(KERNELS
    stubsys
)

if(${DEPLOYABLE})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/kernels)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/kernels
        DESTINATION share/dcpu
        DIRECTORY_PERMISSIONS
            OWNER_READ GROUP_READ WORLD_READ
            OWNER_WRITE GROUP_WRITE WORLD_WRITE
            OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
endif()

# Loop over each kernel and include it, configuring it
# for a CMake build.
foreach(k ${KERNELS})
    include(${k}/config.cmake)
    set(files_append)
    foreach(f ${files})
        list(APPEND files_append ${k}/${f})
    endforeach()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${k})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${k}/policy DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${k})
    add_dcpu_image(${k} ${files_append})

    if(${DEPLOYABLE})
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/${k})
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/.dirs/${k}
            DESTINATION share/dcpu/kernels
            DIRECTORY_PERMISSIONS
                OWNER_READ GROUP_READ WORLD_READ
                OWNER_WRITE GROUP_WRITE WORLD_WRITE
                OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${k}/${k}.djmp16
            ${CMAKE_CURRENT_BINARY_DIR}/${k}/${k}.dkrn16
            ${CMAKE_CURRENT_BINARY_DIR}/${k}/${k}.dsym16
            ${CMAKE_CURRENT_SOURCE_DIR}/${k}/${k}/policy
            DESTINATION
            share/dcpu/kernels/${k})
    endif()
endforeach()

