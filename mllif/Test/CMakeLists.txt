project(MLLIFTest LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/*.cxx)
add_mllif_library(MLLIFTest CXX ${SOURCES})
add_dependencies(MLLIFTest MLLIFC mllif-mlir)

set_target_properties(MLLIFTest PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME MLLIFTest COMMAND xmllint --noout "$<TARGET_GENEX_EVAL:MLLIFTest,$<TARGET_PROPERTY:MLLIFTest,TARGET_MSM_FILE>>")
