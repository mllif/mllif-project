project(lilac-c LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(lilac-c MODULE ${SOURCES} PLUGIN_TOOL clang)
