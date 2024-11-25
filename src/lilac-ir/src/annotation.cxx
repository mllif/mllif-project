#include "lilac-ir/annotation.h"

#include <lilac-shared/annotation.h>
#include <map>
#include <utility>

auto lilac::ir::RawFunctionAnnotation::Create(llvm::Module &module, const llvm::ConstantStruct *constantStruct) -> std::optional<RawFunctionAnnotation> {
    const auto fnConstant = constantStruct->getOperand(0);
    if (!fnConstant) {
        return std::nullopt;
    }
    const auto fn = module.getFunction(fnConstant->getName());
    if (!fn) {
        llvm::errs()
            << "warn: no function matched for annotation '"
            << fnConstant->getName() << "'\n";
        return std::nullopt;
    }

    const auto annotationConstant = constantStruct->getOperand(1);
    if (!annotationConstant) {
        llvm::errs()
            << "warn: value missing for annotation of function '"
            << fn->getName() << "'\n";
        return std::nullopt;
    }

    const auto annotationVar = module.getGlobalVariable(annotationConstant->getName(), true);
    if (!annotationVar) {
        llvm::errs()
            << "warn: global variable missing for annotation of function '"
            << fn->getName() << "'\n";
        return std::nullopt;
    }

    const auto initializer = dyn_cast<llvm::ConstantDataArray>(annotationVar->getInitializer());
    if (!initializer) {
        llvm::errs()
            << "warn: initializer missing for annotation variable of function '"
            << fn->getName() << "'\n";
        return std::nullopt;
    }
    if (!initializer->isCString()) {
        llvm::errs()
            << "warn: initializer for annotation variable of function '"
            << fn->getName() << "' is not a string\n";
        return std::nullopt;
    }

    return RawFunctionAnnotation{fn, initializer->getAsCString()};
}
auto lilac::ir::RawFunctionAnnotation::CreateVector(llvm::Module &module) -> std::vector<RawFunctionAnnotation> {
    std::vector<RawFunctionAnnotation> annotations;

    for (const auto &global : module.globals()) {
        if (global.getName() != "llvm.global.annotations") {
            continue;
        }

        const auto array = dyn_cast<llvm::ConstantArray>(global.getOperand(0));
        if (!array) {
            continue;
        }

        for (const auto &operand : array->operands()) {
            const auto constantStruct = dyn_cast<llvm::ConstantStruct>(operand.get());
            if (auto annotation = Create(module, constantStruct)) {
                annotations.push_back(annotation.value());
            }
        }
    }

    return annotations;
}
auto lilac::ir::ParameterAnnotation::ParseCallInst(llvm::Module &module, llvm::Function *fn, llvm::CallInst *call) -> std::optional<std::pair<std::string, std::string>> {
    const auto callee = call->getCalledFunction();
    if (!callee->isIntrinsic() ||
        !callee->getName().starts_with("llvm.var.annotation")) {
        return std::nullopt;
    }

    const auto argName = call->getOperand(0)->getName();

    size_t argIndex = 0;
    for (; argIndex < callee->arg_size(); ++argIndex) {
        if (callee->getArg(argIndex)->getName() == argName) {
            break;
        }
    }
    if (argIndex == callee->arg_size()) {
        llvm::errs()
            << "warn: annotated parameter '"
            << argName
            << "' not exists in function '"
            << fn->getName()
            << "'\n";
        return std::nullopt;
    }

    const auto annotationVarName = call->getOperand(1)->getName();
    const auto annotationVar = module.getGlobalVariable(annotationVarName, true);
    if (!annotationVar) {
        llvm::errs()
            << "warn: global variable missing for annotation of parameter '"
            << argName
            << "' of function '"
            << fn->getName() << "'\n";
        return std::nullopt;
    }

    const auto initializer = dyn_cast<llvm::ConstantDataArray>(annotationVar->getInitializer());
    if (!initializer) {
        llvm::errs()
            << "warn: initializer missing for annotation variable of parameter '"
            << argName
            << "' of function '"
            << fn->getName() << "'\n";
        return std::nullopt;
    }
    if (!initializer->isCString()) {
        llvm::errs()
            << "warn: initializer for annotation variable of parameter '"
            << argName
            << "' of function '"
            << fn->getName() << "' is not a string\n";
        return std::nullopt;
    }

    const std::string annotation(initializer->getAsCString());
    return shared::ParseAnnotation(annotation);
}
auto lilac::ir::ParameterAnnotation::Create(llvm::Module &module, llvm::Function *fn) -> std::optional<ParameterAnnotation> {
    auto ret = 0U;

    // NOLINTNEXTLINE(*-const-correctness)
    std::string type, name;
    for (auto& block : *fn) {
        for (auto& inst : block) {
            const auto call = dyn_cast<llvm::CallInst>(&inst);
            if (!call) {
                continue;
            }

            const auto pair = ParseCallInst(module, fn, call);
            if (!pair) {
                continue;
            }

            if (auto [key, value] = pair.value(); key == shared::TYPE) {
                type = value;
                ret |= 1;
            } else if (key == shared::NAME) {
                name = value;
                ret |= 2;
            } else {
                llvm::errs()
                    << "warn: unrecognized annotation key '"
                    << key
                    << "' of a parameter of function '"
                    << fn->getName()
                    << "' will be ignored.\n";
            }
        }
    }

    if ((ret & 1U) == 0) {
        llvm::errs() << "warn: type missing for a parameter of function '" << fn->getName() << "'\n";
    }
    if ((ret & 2U) == 0) {
        llvm::errs() << "warn: name missing for a parameter of function '" << fn->getName() << "'\n";
    }
    if (ret != 3) {
        return std::nullopt;
    }

    return ParameterAnnotation{type, name};
}
auto lilac::ir::FunctionAnnotation::Create(llvm::Function *function, const std::vector<llvm::StringRef> &annotations) -> std::optional<FunctionAnnotation> {
    auto ret = 0U;

    std::string ns, name, callconv, retT;
    for (const auto &annotation : annotations) {
        auto parsed = shared::ParseAnnotation(annotation.str());
        if (!parsed) {
            continue;
        }

        auto [key, value] = parsed.value();

        if (key == shared::NAMESPACE) {
            ns = value;
            ret |= 1U;
        } else if (key == shared::NAME) {
            name = value;
            ret |= 1U << 1U;
        } else if (key == shared::CALLCONV) {
            callconv = value;
            ret |= 1U << 2U;
        } else if (key == shared::RETURN) {
            retT = value;
            ret |= 1U << 3U;
        } else {
            llvm::errs()
                << "warn: unrecognized annotation key '"
                << key
                << "' for function '"
                << function->getName()
                << "' will be ignored.\n";
        }
    }

    if ((ret & 1U) == 0) {
        llvm::errs() << "warn: namespace missing for function '" << function->getName() << "'\n";
    }
    if ((ret & 1U << 1U) == 0) {
        llvm::errs() << "warn: name missing for function '" << function->getName() << "'\n";
    }
    if ((ret & 1U << 2U) == 0) {
        llvm::errs() << "warn: callconv missing for function '" << function->getName() << "'\n";
    }
    if ((ret & 1U << 3U) == 0) {
        llvm::errs() << "warn: return type missing for function '" << function->getName() << "'\n";
    }
    if (constexpr unsigned ALL = 0b1111; ret != ALL) {
        return std::nullopt;
    }

    return FunctionAnnotation{function, ns, name, callconv, retT};
}
auto lilac::ir::FunctionAnnotation::CreateVector(const std::vector<RawFunctionAnnotation> &annotations) -> std::vector<FunctionAnnotation> {

    std::map<llvm::Function *, std::vector<llvm::StringRef>> map;
    for (auto annotation : annotations) {
        map[annotation.getFunction()].push_back(annotation.getValue());
    }

    std::vector<FunctionAnnotation> ret;
    for (const auto &[fn, annotations] : map) {
        if (auto annotation = Create(fn, annotations)) {
            ret.push_back(annotation.value());
        }
    }

    return ret;
}