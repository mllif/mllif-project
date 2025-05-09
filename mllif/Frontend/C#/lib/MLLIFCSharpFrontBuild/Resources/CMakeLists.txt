cmake_minimum_required(VERSION 3.21)
project(library LANGUAGES CXX)


set(RID linux-x64)


set(RVR "{@}")
set(CMAKE_CXX_STANDARD 17)

function(PrepareNetHost)
    set(ARCHIVE_URL "https://www.nuget.org/api/v2/package/runtime.${RID}.Microsoft.NETCore.DotNetAppHost/${RVR}")
    set(ARCHIVE_NAME "runtime.zip")
    set(ARCHIVE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dep")
    set(ARCHIVE_PATH "${ARCHIVE_DIR}/${ARCHIVE_NAME}")
    set(HEADER_PATH "${ARCHIVE_DIR}/runtimes/${RID}/native")
    
    file(MAKE_DIRECTORY "${ARCHIVE_DIR}")
    file(DOWNLOAD "${ARCHIVE_URL}" "${ARCHIVE_PATH}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${ARCHIVE_PATH}"
        WORKING_DIRECTORY "${ARCHIVE_DIR}"
    )
    
    include_directories("${HEADER_PATH}")
    link_directories("${HEADER_PATH}")
    
    if (RID MATCHES "win$")
        set(NETHOST_SO "${HEADER_PATH}/nethost.dll" CACHE INTERNAL "")
        set(NETHOST "${HEADER_PATH}/nethost.lib" CACHE INTERNAL "")
        set(HOSTFXR "${HEADER_PATH}/hostfxr.lib" CACHE INTERNAL "")
    elseif (RID MATCHES "osx$")
        set(NETHOST_SO "${HEADER_PATH}/libnethost.dylib" CACHE INTERNAL "")
        set(NETHOST "${HEADER_PATH}/libnethost.a" CACHE INTERNAL "")
        set(HOSTFXR "${HEADER_PATH}/libhostfxr.a" CACHE INTERNAL "")
    else ()
        set(NETHOST_SO "${HEADER_PATH}/libnethost.so" CACHE INTERNAL "")
        set(NETHOST "${HEADER_PATH}/libnethost.a" CACHE INTERNAL "")
        set(HOSTFXR "${HEADER_PATH}/libhostfxr.a" CACHE INTERNAL "")
    endif ()
endfunction()

PrepareNetHost()

add_library(library SHARED library.cxx library.h)
target_link_libraries(library PUBLIC "${NETHOST_SO}" "${NETHOST}" "${HOSTFXR}")
target_include_directories(library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
