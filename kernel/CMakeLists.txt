cmake_minimum_required(VERSION 2.8.5)

include(dcpu.cmake)

# Define a list of kernel folders that we know about.
set(KERNELS
    stubsys
)

# Loop over each kernel and include it, configuring it
# for a CMake build.
foreach(k ${KERNELS})
    include(${k}/config.cmake)
    set(files_append)
    foreach(f ${files})
        list(APPEND files_append ${k}/${f})
    endforeach()
    add_dcpu_image(${k} ${files_append})
    # TODO: Something with name / description / supported.
endforeach()
