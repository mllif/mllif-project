#include "pch.h"
#include <mllif/Frontend/MLIR/Adapter.h>
#include <mllif/Frontend/MLIR/Tree.h>
#include <mllif/Frontend/annotation.h>

namespace {
    auto LoadModule(mlir::MLIRContext& context, const std::string& filename) -> std::unique_ptr<mlir::ModuleOp> {

        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> fileOrErr = llvm::MemoryBuffer::getFile(filename);
        if (const std::error_code ec = fileOrErr.getError()) {
            llvm::errs() << "error: couldn't open input file: " << ec.message() << "\n";
            return nullptr;
        }

        llvm::SourceMgr sourceManager;
        sourceManager.AddNewSourceBuffer(std::move(*fileOrErr), llvm::SMLoc());
        mlir::OwningOpRef<mlir::ModuleOp> owningModule = mlir::parseSourceFile<mlir::ModuleOp>(sourceManager, &context);
        if (!owningModule) {
            llvm::errs() << "error: couldn't load file " << filename << "\n";
            return nullptr;
        }


        return std::make_unique<mlir::ModuleOp>(owningModule.release());
    }
}

auto main(int argc, char **argv) -> int {
    mlir::MLIRContext context;

    mlir::DialectRegistry registry;
    registry.insert<
#include <mllif/Frontend/MLIR/Dialects.inc>
        >();
    context.appendDialectRegistry(registry);

    if (argc <= 2) {
        llvm::errs() << "usage: mllif-mlir <output> <file>...\n";
        return 1;
    }
    const std::string output = argv[1];

    mllif::mlir::Tree symbols;

    for (auto i = 2; i < argc; ++i) {
        const std::shared_ptr module = LoadModule(context, std::string(argv[i]));

        module->walk([&module, &symbols](mlir::Operation* op, const mlir::WalkStage& stage) {
            if (!stage.isAfterAllRegions()) {
                return;
            }

            for (const auto adapter : mllif::mlir::Adapters) {
                adapter->handle(symbols, module, op);
            }
        });
    }

    std::error_code error;
    llvm::raw_fd_ostream os(output, error);
    if (error.value()) {
        llvm::errs() << "error: couldn't open file '" << output << "': " << error.message() << "\n";
        return 1;
    }

    symbols.root().print(os);

    return 0;
}
