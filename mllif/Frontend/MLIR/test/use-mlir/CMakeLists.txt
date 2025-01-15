project(MLLIFTest LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cxx)
add_mllif_library(MLLIFTest CXX ${SOURCES})
set_target_properties(MLLIFTest PROPERTIES
        COMPILE_OPTIONS -std=c++26
)
add_dependencies(MLLIFTest MLLIFCFront mllif-mlir)

set_target_properties(MLLIFTest PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME use-mlir COMMAND xmllint --noout "$<TARGET_GENEX_EVAL:MLLIFTest,$<TARGET_PROPERTY:MLLIFTest,TARGET_MSM_FILE>>")
