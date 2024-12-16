project(mllic-c LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_library(mllic-c MODULE ${SOURCES} PLUGIN_TOOL clang)
target_link_libraries(mllic-c PUBLIC mllic-shared)
target_precompile_headers(mllic-c PUBLIC pch.h)

add_test(NAME mllic-c COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")