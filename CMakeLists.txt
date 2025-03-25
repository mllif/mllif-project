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

function(mllif_target target)
    get_property(tmp GLOBAL PROPERTY MLLIF_TARGET_LIST)
    list(APPEND tmp ${target})
    set_property(GLOBAL PROPERTY MLLIF_TARGET_LIST ${tmp})
endfunction()

add_subdirectory(mllif)

get_property(MLLIF_TARGET_LIST GLOBAL PROPERTY MLLIF_TARGET_LIST)
message(STATUS "Selected Components: ${MLLIF_TARGET_LIST}")

install(FILES cmake/UseMLLIF.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
install(TARGETS ${MLLIF_TARGET_LIST}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        FILE_SET HEADERS DESTINATION include
)
