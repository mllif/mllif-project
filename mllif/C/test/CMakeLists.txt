list_subdirectory(SUBDIRS "${CMAKE_CURRENT_LIST_DIR}")
foreach (SUBDIR ${SUBDIRS})
    add_subdirectory(${SUBDIR})
endforeach ()