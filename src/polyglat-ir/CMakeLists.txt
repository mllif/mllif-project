project(polyglat-ir LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)

add_llvm_pass_plugin(polyglat-ir ${SOURCES})
target_precompile_headers(polyglat-ir PUBLIC pch.h)
target_link_libraries(polyglat-ir PUBLIC polyglat-shared)

add_test(NAME polyglat-ir COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")
