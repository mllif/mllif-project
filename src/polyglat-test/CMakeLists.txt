project(polyglat-test LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

file(GLOB_RECURSE SOURCES src/*.*)
add_polyglat_library(polyglat-test CXX ${SOURCES})
add_dependencies(polyglat-test polyglat-c polyglat-ir)
