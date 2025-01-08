project(MLLIFCFront LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_llvm_library(MLLIFCFront MODULE ${SOURCES} ${HEADERS} PLUGIN_TOOL clang)
target_include_directories(MLLIFCFront PUBLIC include)
target_precompile_headers(MLLIFCFront PUBLIC lib/pch.h)
target_link_libraries(MLLIFCFront PUBLIC MLLIFShared)
set_target_properties(MLLIFCFront PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)

add_subdirectory(test)
