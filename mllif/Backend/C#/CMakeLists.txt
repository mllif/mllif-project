project(mllif-cs LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_executable(mllif-cs ${SOURCES} ${HEADERS})
target_link_libraries(mllif-cs PUBLIC MLLIFSharedBack)
target_include_directories(mllif-cs PUBLIC include)
target_include_directories(mllif-cs PUBLIC lib)
target_precompile_headers(mllif-cs PUBLIC lib/pch.h)
set_target_properties(mllif-cs PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)
