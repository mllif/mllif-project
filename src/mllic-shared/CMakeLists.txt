project(mllic-shared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(mllic-shared STATIC ${SOURCES})
target_precompile_headers(mllic-shared PUBLIC pch.h)
