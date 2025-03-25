project(MLLIFSharedBack LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

mllif_target(MLLIFSharedBack)
add_library(MLLIFSharedBack STATIC ${SOURCES} ${HEADERS})
target_sources(MLLIFSharedBack PUBLIC FILE_SET HEADERS FILES ${HEADERS})

target_include_directories(MLLIFSharedBack PUBLIC include)
target_precompile_headers(MLLIFSharedBack PUBLIC lib/pch.h)
