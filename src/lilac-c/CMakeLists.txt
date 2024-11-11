project(lilac-c LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(lilac-c MODULE ${SOURCES} PLUGIN_TOOL clang)

add_test(NAME lilac-c COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")