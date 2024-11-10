project(lilac-ir LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(lilac-ir MODULE ${SOURCES} PLUGIN_TOOL clang)
