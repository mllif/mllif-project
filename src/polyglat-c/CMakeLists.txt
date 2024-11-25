project(polyglat-c LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(polyglat-c MODULE ${SOURCES} PLUGIN_TOOL clang)
target_link_libraries(polyglat-c PUBLIC polyglat-shared)
target_precompile_headers(polyglat-c PUBLIC pch.h)

add_test(NAME polyglat-c COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")