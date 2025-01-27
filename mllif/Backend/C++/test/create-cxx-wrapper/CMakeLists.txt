add_test(
        NAME use-cxx-wrapper
        COMMAND bash -c "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mllif-cxx < \"$<TARGET_GENEX_EVAL:MLLIFTest,$<TARGET_PROPERTY:MLLIFTest,TARGET_MSM_FILE>>\" > wrapper.h"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)
