add_test(
        NAME create-cs-bridge
        COMMAND bash -c "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mllif-cs MLLIFTest < \"$<TARGET_GENEX_EVAL:MLLIFTest,$<TARGET_PROPERTY:MLLIFTest,TARGET_MSM_FILE>>\" > bridge.cs; dotnet build"
        #"clang -xc++-header wrapper.h -o a.out;"
        #"rm a.out wrapper.h;"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)
