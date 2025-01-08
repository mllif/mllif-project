project(MLLIFMLIR LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

add_executable(mllif-mlir ${SOURCES} ${HEADERS})
target_include_directories(mllif-mlir PUBLIC include)
target_precompile_headers(mllif-mlir PUBLIC lib/pch.h)
set_target_properties(mllif-mlir PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${HEADERS}"
)

target_link_libraries(mllif-mlir PUBLIC
        MLLIFSharedFront
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

add_subdirectory(test)
