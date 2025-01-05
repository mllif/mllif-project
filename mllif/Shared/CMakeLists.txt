project(MLLIFShared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(MLLIFShared STATIC ${SOURCES})
target_precompile_headers(MLLIFShared PUBLIC pch.h)
