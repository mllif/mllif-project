project(MLLIFMLIR LANGUAGES CXX)

file(GLOB_RECURSE SOURCES *.cxx)

add_executable(mllif-mlir ${SOURCES})
target_precompile_headers(mllif-mlir PUBLIC pch.h)

target_link_libraries(mllif-mlir PUBLIC
        MLLIFShared
        MLIRDialect
        MLIRDialectUtils
        MLIRTransforms
        MLIRTransformUtils
        MLIRAnalysis
        MLIRParser
        MLIRPass
        MLIRLLVMDialect
        MLIRMemRefDialect
        MLIRSideEffectInterfaces
        MLIROptLib
        MLIRBytecodeWriter
        MLIRBytecodeReader
        MLIRBytecodeOpInterface
        MLIRPluginsLib
        MLIRIRDL
        MLIRCIR
        MLIRCIRInterfaces
        MLIRCIRTransforms
        MLIRDebug
        MLIRIR
        MLIRObservers
        MLIRSupport
        clang-cpp
        clangCIR
        clangCIRLoweringThroughMLIR
        clangCIRLoweringDirectToLLVM
        LLVMSupport
)

#add_test(NAME mllif-cir COMMAND "${CMAKE_CURRENT_LIST_DIR}/test/test.sh" "${CMAKE_BINARY_DIR}")