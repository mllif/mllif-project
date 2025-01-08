project(MLLIFC LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_llvm_library(MLLIFC MODULE ${SOURCES} ${HEADERS} PLUGIN_TOOL clang)
target_include_directories(MLLIFC PUBLIC include)
target_precompile_headers(MLLIFC PUBLIC lib/pch.h)
target_link_libraries(MLLIFC PUBLIC MLLIFShared)

add_subdirectory(test)
