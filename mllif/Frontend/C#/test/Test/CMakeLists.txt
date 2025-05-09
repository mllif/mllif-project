cmake_minimum_required(VERSION 3.21)
project(test LANGUAGES CXX)

add_subdirectory(br)

file(COPY "${NETHOST_SO}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(test main.cxx)
target_link_libraries(test PUBLIC library)
