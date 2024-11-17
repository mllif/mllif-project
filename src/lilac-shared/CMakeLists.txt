project(lilac-shared LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.*)
add_library(lilac-shared STATIC ${SOURCES})

### seungwoo baka
### tasukete tau