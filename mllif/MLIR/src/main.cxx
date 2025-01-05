#include "mllif/MLIR/pch.h"

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <mlir/Parser/Parser.h>
#include <mllif/MLIR/symboltree.h>
#include <mllif/Shared/annotation.h>

namespace {
    std::unique_ptr<mlir::ModuleOp> LoadModule(mlir::MLIRContext& context, const std::string& filename) {

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

    template<typename T>
    bool Store(const mlir::ArrayAttr& array, std::string from, std::string name, T& container) {
        bool success = true;
        for (auto value : array) {
            const auto str = mlir::dyn_cast<mlir::StringAttr>(value);
            if (!str) {
                llvm::errs() << "error: " << name << " should be string (" << from << ")\n";
                success = false;
                continue;
            }

            container.emplace_back(str.getValue());
        }

        return success;
    }
}

auto main(int argc, char **argv) -> int {
    mlir::MLIRContext context;

    mlir::DialectRegistry registry;
    registry.insert<
        mlir::BuiltinDialect,
        mlir::arith::ArithDialect,
        cir::CIRDialect,
        mlir::memref::MemRefDialect,
        mlir::LLVM::LLVMDialect,
        mlir::DLTIDialect,
        mlir::omp::OpenMPDialect>();
    context.appendDialectRegistry(registry);

    if (argc <= 2) {
        llvm::errs() << "usage: mllif-mlir <output> <file>...\n";
        return 1;
    }
    const std::string output = argv[1];


    mllif::mlir::SymbolTree tree;

    for (auto i = 2; i < argc; ++i) {
        auto module = LoadModule(context, std::string(argv[i]));

        module->walk([&module, &tree](mlir::Operation* op, const mlir::WalkStage& stage) {
            auto fn = mlir::dyn_cast<cir::FuncOp>(op);
            if (!fn || !stage.isAfterAllRegions()) {
                return;
            }

            const auto annotations = fn.getAnnotations();
            if (!annotations || annotations->empty()) {
                return;
            }

            std::deque<std::string> path;
            std::string tag;
            bool success = true;

            for (auto attribute : annotations.value()) {
                auto annotation = mlir::dyn_cast<cir::AnnotationAttr>(attribute);
                if (!annotation) {
                    continue;
                }

                auto key = annotation.getName().getValue();
                if (!key.starts_with(mllif::shared::Namespace + '.')) {
                    continue;
                }

                key = key.substr(mllif::shared::Namespace.size() + 1);

                if (key == mllif::shared::prefix::Path) {
                    success &= Store(annotation.getArgs(), fn.getSymName().str(), "path segment", path);

                } else if (key == mllif::shared::prefix::Type) {
                    const auto args = annotation.getArgs().getValue();
                    if (args.size() < 1) {
                        llvm::errs() << "error: insufficient argument number: function type should be specified for function '" << fn.getSymName() << "'\n";
                        success = false;
                        continue;
                    }

                    const auto str = mlir::dyn_cast<mlir::StringAttr>(args[0]);
                    if (!str) {
                        llvm::errs() << "error: function type should be string (" << fn.getSymName() << ")\n";
                        success = false;
                        continue;
                    }

                    tag = str.getValue();

                } else {
                    llvm::errs() << "error: unrecognized key '" << key << "' for function '" << fn.getSymName() << "'\n";
                    success = false;
                }
            }

            if (!success) {
                llvm::errs() << "error: function '" << fn.getSymName() << "' has invalid annotations; couldn't be exported\n";
                return;
            }


            const auto args = fn.getArguments();

            std::vector<std::string> argNames;
            argNames.reserve(args.size());
            fn.getBody().walk([&argNames](mlir::Operation* op) {
                if (auto alloca = llvm::dyn_cast<cir::AllocaOp>(op); alloca && alloca.getInit()) {
                    argNames.push_back(alloca.getName().str());
                }
            });

            if (args.size() != argNames.size()) {
                llvm::errs() << "error: invalid function signature for '" << fn.getSymName() << "' (annotation mismatched)\n";
                return;
            }

            auto fnSym = tree.insert(path);
            fnSym.tag = tag;

            for (auto iParm = 0; iParm < args.size(); ++iParm) {
                std::string buffer;
                llvm::raw_string_ostream os(buffer);
                args[iParm].getType().print(os);
                os.flush();

                auto parmSym = mllif::mlir::Symbol("param", argNames[iParm]);
                parmSym.attributes.emplace_back("type", buffer);
                fnSym.children.push_back(parmSym);
            }
        });
    }

    std::error_code error;
    llvm::raw_fd_ostream os(output, error);
    if (error.value()) {
        llvm::errs() << "error: couldn't open file '" << output << "': " << error.message() << "\n";
        return 1;
    }

    tree.print(os);

    return 0;
}
