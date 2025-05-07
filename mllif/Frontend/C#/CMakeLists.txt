project(MLLIFCSharpFront)

add_custom_target(MLLIFCSharpFront ALL
        COMMAND dotnet build --artifacts-path "${CMAKE_CURRENT_BINARY_DIR}/lib/MLLIFCSharpFront" --output "${CMAKE_CURRENT_BINARY_DIR}/lib"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib"
)

add_custom_target(MLLIFCSharpFrontBuild ALL
        COMMAND dotnet build --artifacts-path "${CMAKE_CURRENT_BINARY_DIR}/lib/MLLIFCSharpFrontBuild" --output "${CMAKE_CURRENT_BINARY_DIR}/lib"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib"
)

add_test(
        NAME use-csharp-frontend
        COMMAND dotnet run --artifacts-path "${CMAKE_CURRENT_BINARY_DIR}/test" --output "${CMAKE_CURRENT_BINARY_DIR}/test"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/test"
)
