project(MLLIFC LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cxx)

add_llvm_library(MLLIFC MODULE ${SOURCES} PLUGIN_TOOL clang)
target_link_libraries(MLLIFC PUBLIC MLLIFShared)
target_precompile_headers(MLLIFC PUBLIC pch.h)
