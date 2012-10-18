cmake_minimum_required(CMAKE 2.8.6)

include(dcpu)

# Define a list of kernel folders that we know about.
set(KERNELS
    stubsys
)

# Loop over each kernel and include it, configuring it
# for a CMake build.
foreach(k ${KERNELS})
    include(${k}/config)
    add_dcpu_image(${k} ${files})
    # TODO: Something with name / description / supported.
endforeach()
