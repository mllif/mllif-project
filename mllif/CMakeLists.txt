macro(list_subdirectory result current)
    file(GLOB children RELATIVE ${current} ${current}/*)
    set(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${current}/${child})
            list(APPEND dirlist ${child})
        endif ()
    endforeach ()
    set(${result} ${dirlist})
endmacro()

list_subdirectory(SUBDIRS "${CMAKE_CURRENT_LIST_DIR}")
foreach (SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
endforeach ()
