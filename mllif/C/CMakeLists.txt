project(MLLIFC LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(MLLIFC MODULE ${SOURCES} PLUGIN_TOOL clang)
target_link_libraries(MLLIFC PUBLIC MLLIFShared)
target_precompile_headers(MLLIFC PUBLIC pch.h)

add_test(NAME MLLIFC COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")