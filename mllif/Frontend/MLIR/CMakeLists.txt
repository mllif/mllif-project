project(MLLIFMLIR LANGUAGES CXX)

file(GLOB_RECURSE SOURCES lib/*.*)
file(GLOB_RECURSE HEADERS include/*.*)

mllif_target(mllif-mlir)
add_executable(mllif-mlir ${SOURCES} ${HEADERS})
target_sources(mllif-mlir PUBLIC FILE_SET HEADERS FILES ${HEADERS})

target_include_directories(mllif-mlir PUBLIC include)
target_include_directories(mllif-mlir PRIVATE lib)
target_precompile_headers(mllif-mlir PUBLIC lib/pch.h)

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
