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

#include "polyglat-ir/pch.h"

#include "polyglat-ir/annotation.h"

class IRCollectPass : public llvm::PassInfoMixin<IRCollectPass> {
  public:
    static auto run(llvm::Module &module, llvm::ModuleAnalysisManager & /**/) -> llvm::PreservedAnalyses;
};

auto IRCollectPass::run(llvm::Module &module, llvm::ModuleAnalysisManager & /**/) -> llvm::PreservedAnalyses {

    const auto rawAnnotations = polyglat::ir::RawFunctionAnnotation::CreateVector(module);
    const auto annotations = polyglat::ir::FunctionAnnotation::CreateVector(rawAnnotations);

    for (const auto &raw : rawAnnotations) {
        llvm::outs() << raw.getFunction()->getName() << ' ' << raw.getValue() << '\n';
    }

    for (const auto &annotation : annotations) {
        llvm::outs() << annotation.getNamespace() << '/' << annotation.getName() << '\n';
    }

    return llvm::PreservedAnalyses::all();
}

extern "C" auto LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() -> llvm::PassPluginLibraryInfo {
    return llvm::PassPluginLibraryInfo{
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "LILAC_IR_COLLECTOR",
        .PluginVersion = __TIME__,
        .RegisterPassBuilderCallbacks = [](llvm::PassBuilder &builder) {
            builder.registerPipelineStartEPCallback(
                [](llvm::ModulePassManager &manager, llvm::OptimizationLevel) {
                    manager.addPass(IRCollectPass());
                    return true;
                });
        }};
}
