add_test(
        NAME create-cxx-bridge
        COMMAND bash -c
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mllif-cxx < \"$<TARGET_GENEX_EVAL:MLLIFTest,$<TARGET_PROPERTY:MLLIFTest,TARGET_MSM_FILE>>\" > bridge.h;"
        "clang -xc++-header bridge.h -o a.out;"
        "rm a.out bridge.h;"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)
