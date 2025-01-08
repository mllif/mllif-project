cmake_minimum_required(VERSION 3.26)
project(mllif)

set(ENABLE_LLVM_SHARED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib/")
set(MLLIF_PATH "${CMAKE_BINARY_DIR}/build/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(UseMLLIF)
include(UseClang)
include(UseLLVM)
include(CTest)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(mllif)

install(TARGETS MLLIFCFront mllif-mlir MLLIFSharedFront
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
