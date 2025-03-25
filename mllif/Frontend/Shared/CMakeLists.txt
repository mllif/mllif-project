project(MLLIFSharedFront LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

mllif_target(MLLIFSharedFront)
add_library(MLLIFSharedFront STATIC ${SOURCES} ${HEADERS})
target_sources(MLLIFSharedFront PUBLIC FILE_SET HEADERS FILES ${HEADERS})

target_include_directories(MLLIFSharedFront PUBLIC include)
target_precompile_headers(MLLIFSharedFront PUBLIC lib/pch.h)
