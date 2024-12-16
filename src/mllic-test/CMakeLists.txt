project(mllic-test LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.cxx)
add_mllic_library(mllic-test CXX ${SOURCES})
add_dependencies(mllic-test mllic-c)

set_target_properties(mllic-test PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME mllic-test COMMAND xmllint --noout "$<TARGET_GENEX_EVAL:mllic-test,$<TARGET_PROPERTY:mllic-test,TARGET_PSM_FILE>>")
