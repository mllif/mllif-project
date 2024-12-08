/*
 * Copyright 2024 Yeong-won Seo
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

#include "polyglat-ir/annotation.h"
#include "polyglat-ir/pch.h"

#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Errc.h>
#include <llvm/Support/FileSystem.h>

namespace {
    // NOLINTNEXTLINE(cert-err58-cpp, cppcoreguidelines-avoid-non-const-global-variables)
    llvm::cl::OptionCategory Category("Polyglat Options", "Configure the Polyglat system");

    [[maybe_unused]]
    // NOLINTNEXTLINE(cert-err58-cpp)
    const llvm::cl::opt<std::string> OutputFilePath(
        "polyglat-output",
        llvm::cl::desc("Place polyglat symbols into <file>"),
        llvm::cl::value_desc("file"),
        llvm::cl::cat(Category),
        llvm::cl::Required,
        llvm::cl::ValueRequired);
} // namespace

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
class PolyglatPass final : public llvm::PassInfoMixin<PolyglatPass> {
    std::vector<polyglat::ir::FunctionAnnotation> Annotations;

  public:
    auto run(llvm::Module &module, llvm::ModuleAnalysisManager & /**/) -> llvm::PreservedAnalyses;

    ~PolyglatPass();
};

auto PolyglatPass::run(llvm::Module &module, llvm::ModuleAnalysisManager & /**/) -> llvm::PreservedAnalyses {
    const auto rawAnnotations = polyglat::ir::RawFunctionAnnotation::CreateVector(module.getContext(), module);
    Annotations = polyglat::ir::FunctionAnnotation::CreateVector(module.getContext(), rawAnnotations);

    if (Annotations.empty()) {
    }

    return llvm::PreservedAnalyses::all();
}

static constexpr auto Success = static_cast<std::errc>(0);

PolyglatPass::~PolyglatPass() {
    if (OutputFilePath.empty()) {
        llvm::report_fatal_error("Output file path of polyglat cannot be empty. Did you specified '--polyglat-output'?", false);
        // return
    }

    std::error_code err;
    llvm::raw_fd_ostream os(
        OutputFilePath.getValue(),
        err,
        llvm::sys::fs::CD_CreateAlways,
        llvm::sys::fs::FA_Write,
        llvm::sys::fs::OF_None);
    if (err != Success) {
        report_fatal_error(llvm::StringRef(err.message()), false);
        // return
    }

    for (const auto &annotation : Annotations) {
        os << annotation.getNamespace() << '/' << annotation.getName() << '\n';
    }
}

extern "C" auto LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() -> llvm::PassPluginLibraryInfo {
    std::cerr << "Polyglat IR collector plugin\n";
    return llvm::PassPluginLibraryInfo{
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "polyglat",
        .PluginVersion = __TIME__,
        .RegisterPassBuilderCallbacks = [](llvm::PassBuilder &PB) {
            std::cerr << "Polyglat IR collector plugin 222\n";

            // #1 REGISTRATION FOR "opt -passes=print<opcode-counter>"
            PB.registerPipelineParsingCallback(
                [&](const llvm::StringRef Name, llvm::ModulePassManager &MAM, llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    if (Name == "polyglat") {
                        MAM.addPass(PolyglatPass());
                        return true;
                    }
                    return false;
                });

            /*
            // #2 REGISTRATION FOR "FAM.getResult<IRCollectPass>(Func)"
            PB.registerAnalysisRegistrationCallback([](llvm::ModuleAnalysisManager &MAM) {
                MAM.registerPass([&] { return PolyglatPass(); });
            });
            */
        }};
}
