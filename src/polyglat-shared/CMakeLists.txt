project(polyglat-shared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(polyglat-shared STATIC ${SOURCES})
target_precompile_headers(polyglat-shared PUBLIC pch.h)
