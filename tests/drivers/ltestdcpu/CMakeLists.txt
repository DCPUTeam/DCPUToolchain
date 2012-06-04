cmake_minimum_required(VERSION 2.8.5)
project(ltestdcpu)

# Check to ensure that we are running as part of the
# root CMakeLists.txt file.
if(NOT DEFINED IS_ROOT)
  message(FATAL_ERROR "You must generate the CMake files as part of the root directory.")
endif(NOT DEFINED IS_ROOT)

# Define the assembler project.
dt_project(executable)
dt_needs(argtable2)
dt_needs(bstring)
dt_needs(libdcpu)

# Define the tests.
enable_testing()
add_test(bopen-01 ${OUTPUT_DIR}/ltestdcpu bopen-01)
add_test(bopen-02 ${OUTPUT_DIR}/ltestdcpu bopen-02)
add_test(bread-01 ${OUTPUT_DIR}/ltestdcpu bread-01)
add_test(bread-02 ${OUTPUT_DIR}/ltestdcpu bread-02)
# FIXME: This is a poorly written test.
#add_test(bcopy-01 ${OUTPUT_DIR}/ltestdcpu bcopy-01)
add_test(btell-01 ${OUTPUT_DIR}/ltestdcpu btell-01)
add_test(btell-02 ${OUTPUT_DIR}/ltestdcpu btell-02)

