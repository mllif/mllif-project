project(mllif-cxx LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

mllif_target(mllif-cxx)
add_executable(mllif-cxx ${SOURCES} ${HEADERS})
target_sources(mllif-cxx PUBLIC FILE_SET HEADERS FILES ${HEADERS})

target_link_libraries(mllif-cxx PUBLIC MLLIFSharedBack)
target_include_directories(mllif-cxx PUBLIC include)
target_include_directories(mllif-cxx PRIVATE lib)
target_precompile_headers(mllif-cxx PUBLIC lib/pch.h)

add_subdirectory(test)
