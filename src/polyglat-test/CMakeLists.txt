project(polyglat-test LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.cxx)
add_polyglat_library(polyglat-test CXX ${SOURCES})
add_dependencies(polyglat-test polyglat-c)

set_target_properties(polyglat-test PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME polyglat-test COMMAND xmllint --noout "$<TARGET_GENEX_EVAL:polyglat-test,$<TARGET_PROPERTY:polyglat-test,TARGET_PSM_FILE>>")
