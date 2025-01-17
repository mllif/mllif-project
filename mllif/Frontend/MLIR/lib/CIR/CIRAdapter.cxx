/*
 * Copyright 2025 Yeong-won Seo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pch.h"

#include <mllif/Frontend/MLIR/Adapter.h>
#include <mllif/Frontend/MLIR/CIR/CIRAdapter.h>
#include <mllif/Frontend/MLIR/TypeReg.h>
#include <mllif/Frontend/annotation.h>

namespace {
    auto GetArgNames(cir::FuncOp &fn) -> std::vector<std::string> {

        std::vector<std::string> argNames;
        argNames.reserve(fn.getNumArguments());

        fn.getBody().walk([&argNames](mlir::Operation *op) {
            if (auto alloca = llvm::dyn_cast<cir::AllocaOp>(op); alloca && alloca.getInit()) {
                argNames.push_back(alloca.getName().str());
            }
        });

        return argNames;
    }
} // namespace

struct CIRAnnotatedData {
    std::deque<std::string> Path;
    std::string Tag;
    bool Skip;
    bool Success;

    explicit CIRAnnotatedData(cir::FuncOp &fn) : Skip(true), Success(true) {
        if (const auto annot = fn.getAnnotations(); !annot || annot.value().empty()) {
            return;
        }

        for (auto attr : fn.getAnnotationsAttr()) {
            auto annotAttr = mlir::dyn_cast<cir::AnnotationAttr>(attr);
            if (!annotAttr) {
                continue;
            }

            mllif::shared::Annotation annot{annotAttr.getName().str()};
            if (!annot.Key.starts_with(mllif::shared::Namespace + '.')) {
                continue;
            }
            annot.Key = annot.Key.substr(mllif::shared::Namespace.size() + 1);

            Skip = false;

            if (annot.Key == mllif::shared::prefix::Path) {
                Path.assign(annot.Values.begin(), annot.Values.end());

            } else if (annot.Key == mllif::shared::prefix::Type) {
                if (annot.Values.size() != 1) {
                    llvm::errs() << "error: type of function should be specified once (" << fn.getSymName() << ")\n";
                    Success = false;
                    continue;
                }

                Tag = annot.Values.front();

            } else {
                llvm::errs() << "error: unrecognized key '" << annot.Key << "' for function '" << fn.getSymName() << "'\n";
                Success = false;
            }
        }
    }
};

void mllif::mlir::cir::CIRAdapter::handle(Tree &symbols, std::shared_ptr<::mlir::ModuleOp> module, ::mlir::Operation *op) {
    auto fn = dyn_cast<::cir::FuncOp>(op);
    if (!fn) {
        return;
    }

    CIRAnnotatedData annotated{fn};
    if (annotated.Skip) {
        return;
    }
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

    auto iParm = 0;

    if (annotated.Tag == shared::type::Method) {
        annotated.Path.pop_back();
        const auto parentNode = symbols.root().insert_inplace(annotated.Path, "");
        assert(parentNode && "parent of a symbol cannot be null");

        // Only object can hold methods
        parentNode->tag() = shared::type::Object;

        // Skip 'this' parameter
        iParm++;
    }

    for (; iParm < args.size(); ++iParm) {
        auto type = Types::From(args[iParm].getType(), module);
        if (!type) {
            llvm::errs() << "error: unrecognized type '";
            args[iParm].getType().print(llvm::errs());
            llvm::errs() << "'\n";
            return;
        }
        const auto typeStr = type->store(symbols);

        fnSym
            ->children()
            .emplace_back("param", argNames[iParm])
            .attributes()
            .emplace_back("type", typeStr);
    }
}

namespace {
    [[maybe_unused]]
    const mllif::mlir::AdapterRegistry<mllif::mlir::cir::CIRAdapter> X{};
}
