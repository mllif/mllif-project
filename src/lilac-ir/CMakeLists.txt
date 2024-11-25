project(lilac-ir LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_pass_plugin(lilac-ir ${SOURCES})
target_precompile_headers(lilac-ir PUBLIC pch.h)
target_link_libraries(lilac-ir PUBLIC lilac-shared)

add_test(NAME lilac-ir COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")
