#include "pch.h"

#include <mllif/Frontend/MLIR/Adapter.h>
#include <mllif/Frontend/MLIR/CIR/CIRAdapter.h>
#include <mllif/Frontend/annotation.h>

namespace {
    template<typename T>
    auto Store(const mlir::ArrayAttr& array, const std::string& from, const std::string& name, T& container) -> bool {
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

    std::vector<std::string> GetArgNames(cir::FuncOp& fn) {

        std::vector<std::string> argNames;
        argNames.reserve(fn.getNumArguments());

        fn.getBody().walk([&argNames](mlir::Operation *op) {
            if (auto alloca = llvm::dyn_cast<cir::AllocaOp>(op); alloca && alloca.getInit()) {
                argNames.push_back(alloca.getName().str());
            }
        });

        return argNames;
    }
}

struct CIRAnnotatedData {
    std::deque<std::string> Path;
    std::string Tag;
    bool Skip;
    bool Success;

    explicit CIRAnnotatedData(cir::FuncOp& fn) : Skip(true), Success(true) {
        if (const auto annot = fn.getAnnotations(); !annot || annot.value().empty()) {
            return;
        }

        for (auto attr : fn.getAnnotationsAttr()) {
            auto annotation = mlir::dyn_cast<cir::AnnotationAttr>(attr);
            if (!annotation) continue;


            auto key = annotation.getName().getValue();
            if (!key.starts_with(mllif::shared::Namespace + '.')) {
                continue;
            }
            key = key.substr(mllif::shared::Namespace.size() + 1);

            Skip = false;

            if (key == mllif::shared::prefix::Path) {
                Success &= Store(annotation.getArgs(), fn.getSymName().str(), "path segment", Path);

            } else if (key == mllif::shared::prefix::Type) {
                const auto args = annotation.getArgs().getValue();
                if (args.size() < 1) {
                    llvm::errs() << "error: insufficient argument number: function type should be specified for function '" << fn.getSymName() << "'\n";
                    Success = false;
                    continue;
                }

                const auto str = dyn_cast<::mlir::StringAttr>(args[0]);
                if (!str) {
                    llvm::errs() << "error: function type should be string (" << fn.getSymName() << ")\n";
                    Success = false;
                    continue;
                }

                Tag = str.getValue();

            } else {
                llvm::errs() << "error: unrecognized key '" << key << "' for function '" << fn.getSymName() << "'\n";
                Success = false;
            }
        }
    }
};

void mllif::mlir::cir::CIRAdapter::handle(Tree &symbols, std::shared_ptr<::mlir::ModuleOp> module, ::mlir::Operation *op) {
    auto fn = dyn_cast<::cir::FuncOp>(op);
    if (!fn) return;

    CIRAnnotatedData annotated{ fn };
    if (annotated.Skip) return;
    if (!annotated.Success) {
        llvm::errs() << "error: function '" << fn.getSymName() << "' has invalid annotations\n";
        return;
    }

    const auto args = fn.getArguments();

    std::vector<std::string> argNames = GetArgNames(fn);

    if (args.size() != argNames.size()) {
        llvm::errs() << "error: invalid function signature for '" << fn.getSymName() << "' (annotation mismatched)\n";
        return;
    }

    auto copyPath = annotated.Path;
    const auto fnSym = symbols.root().insert_inplace(copyPath, annotated.Tag);
    if (!fnSym) {
        llvm::errs() << "error: couldn't create symbol node (";
        for (auto seg : annotated.Path) {
            llvm::errs() << '/' << seg;
        }
        llvm::errs() << "). is there a invalid path for symbol?\n";
        return;
    }

    if (annotated.Tag == shared::type::Method) {
        annotated.Path.pop_back();
        const auto parentNode = symbols.root().insert_inplace(annotated.Path, "");
        assert(parentNode && "parent of a symbol cannot be null");

        // Only object can hold methods
        parentNode->tag() = shared::type::Object;
    }

    for (auto iParm = 0; iParm < args.size(); ++iParm) {
        std::string buffer;
        llvm::raw_string_ostream os(buffer);
        args[iParm].getType().print(os);
        os.flush();

        fnSym
            ->children()
            .emplace_back("param", argNames[iParm])
            .attributes()
            .emplace_back("type", buffer);
    }
}

namespace {
    [[maybe_unused]]
    mllif::mlir::AdapterRegistry<mllif::mlir::cir::CIRAdapter> X{};
}
