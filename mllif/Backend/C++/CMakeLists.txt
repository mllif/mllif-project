project(mllif-cxx LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_executable(mllif-cxx ${SOURCES} ${HEADERS})
target_include_directories(mllif-cxx PUBLIC include)
target_include_directories(mllif-cxx PRIVATE lib)
target_precompile_headers(mllif-cxx PUBLIC lib/pch.h)
set_target_properties(mllif-cxx PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)
