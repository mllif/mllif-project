project(MLLIFShared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_library(MLLIFShared STATIC ${SOURCES} ${HEADERS})
target_include_directories(MLLIFShared PUBLIC include)
target_precompile_headers(MLLIFShared PUBLIC lib/pch.h)
set_target_properties(MLLIFC PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)
