project(mllif-cs LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

mllif_target(mllif-cs)
add_executable(mllif-cs ${SOURCES} ${HEADERS})
target_sources(mllif-cs PUBLIC FILE_SET HEADERS FILES ${HEADERS})

target_link_libraries(mllif-cs PUBLIC MLLIFSharedBack)
target_include_directories(mllif-cs PUBLIC include)
target_include_directories(mllif-cs PUBLIC lib)
target_precompile_headers(mllif-cs PUBLIC lib/pch.h)

add_subdirectory(test)
