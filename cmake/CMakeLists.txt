cmake_minimum_required (VERSION 2.6)
project(AtlasOS)

# Define the location of the toolchain.
set(DCPUTOOLCHAIN ../0x10c/Debug/)

# Include DCPU-16 Toolchain.
include(DCPUToolchain.cmake)

# Define AtlasOS.
add_dcpu_image(AtlasOS
	kernel/core.dasm16
)

