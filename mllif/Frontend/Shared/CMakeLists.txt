project(MLLIFSharedFront LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_library(MLLIFSharedFront STATIC ${SOURCES} ${HEADERS})
target_include_directories(MLLIFSharedFront PUBLIC include)
target_precompile_headers(MLLIFSharedFront PUBLIC lib/pch.h)
set_target_properties(MLLIFSharedFront PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)
