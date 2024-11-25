project(lilac-shared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(lilac-shared STATIC ${SOURCES})
target_precompile_headers(lilac-shared PUBLIC pch.h)
