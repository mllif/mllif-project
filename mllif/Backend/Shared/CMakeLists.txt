project(MLLIFSharedBack LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_library(MLLIFSharedBack STATIC ${SOURCES} ${HEADERS})
target_include_directories(MLLIFSharedBack PUBLIC include)
target_precompile_headers(MLLIFSharedBack PUBLIC lib/pch.h)
set_target_properties(MLLIFSharedBack PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)
