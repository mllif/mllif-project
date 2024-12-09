#include "polyglat-ir/annotation.h"

#include <iostream>
#include <map>
#include <polyglat-ir/diagnostics.h>
#include <polyglat-shared/annotation.h>
#include <utility>

auto polyglat::ir::RawFunctionAnnotation::Create(llvm::LLVMContext &context, llvm::Module &module, const llvm::ConstantStruct *constantStruct) -> std::optional<RawFunctionAnnotation> {
    const auto fnConstant = constantStruct->getOperand(0);
    if (!fnConstant) {
        return std::nullopt;
    }
    const auto fn = module.getFunction(fnConstant->getName());
    if (!fn) {
        context.diagnose(NoSuchFunctionError(fnConstant->getName().str()));
        return std::nullopt;
    }

    const auto annotationConstant = constantStruct->getOperand(1);
    if (!annotationConstant) {
        context.diagnose(ValueRequiredError("annotation of function", fn->getName().str()));
        return std::nullopt;
    }

    const auto annotationVar = module.getGlobalVariable(annotationConstant->getName(), true);
    if (!annotationVar) {
        context.diagnose(NoSuchGlobalVariableError(annotationConstant->getName().str()));
        return std::nullopt;
    }

    const auto initializer = dyn_cast<llvm::ConstantDataArray>(annotationVar->getInitializer());
    if (!initializer || !initializer->isCString()) {
        context.diagnose(InvalidInitializerError("annotation variable of function", fn->getName().str()));
        return std::nullopt;
    }

    return RawFunctionAnnotation{fn, initializer->getAsCString()};
}

auto polyglat::ir::RawFunctionAnnotation::CreateVector(llvm::LLVMContext &context, llvm::Module &module) -> std::vector<RawFunctionAnnotation> {
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
            if (auto annotation = Create(context, module, constantStruct)) {
                annotations.push_back(annotation.value());
            }
        }
    }

    return annotations;
}

auto polyglat::ir::ParameterAnnotation::ParseCallInst(
    llvm::Module &module,
// ReSharper disable once CppParameterMayBeConstPtrOrRef
    llvm::CallInst *call) -> std::optional<std::tuple<std::size_t, std::string, std::string>> {
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
        module.getContext().emitError(call, std::format("Annotated parameter '{}' is not found in function", argName.str()));
        return std::nullopt;
    }

    const auto annotationVarName = call->getOperand(1)->getName();
    const auto annotationVar = module.getGlobalVariable(annotationVarName, true);
    if (!annotationVar) {
        module.getContext().emitError(call, std::format("No such global-variable '{}'", annotationVarName.str()));
        return std::nullopt;
    }

    const auto initializer = dyn_cast<llvm::ConstantDataArray>(annotationVar->getInitializer());
    if (!initializer || !initializer->isCString()) {
        module.getContext().emitError(call, std::format("Invalid initializer of global-variable '{}'", annotationVarName.str()));
        return std::nullopt;
    }

    const std::string annotation(initializer->getAsCString());

    if (auto annot = shared::ParseAnnotation(annotation)) {
        auto [key, value] = *annot;
        return std::make_tuple(argIndex, key, value);
    }

    return std::nullopt;
}

auto polyglat::ir::ParameterAnnotation::Create(llvm::Module &module, llvm::Function *fn) -> std::vector<ParameterAnnotation> {

    std::vector<ParameterAnnotation> annotations{ fn->arg_size() };

    // NOLINTNEXTLINE(*-const-correctness)
    for (auto &block : *fn) {
        for (auto &inst : block) {
            const auto call = dyn_cast<llvm::CallInst>(&inst);
            if (!call) {
                continue;
            }

            const auto pair = ParseCallInst(module, call);
            if (!pair) {
                continue;
            }

            if (auto [index, key, value] = pair.value(); key == shared::TYPE) {
                annotations[index].Type = value;
            } else if (key == shared::NAME) {
                annotations[index].Name = value;
            } else {
                module.getContext().diagnose(UnrecognizedAttributeError(key, "of a parameter of function", fn->getName().str()));
            }
        }
    }

    for (const auto &annotation : annotations) {
        if (annotation.Type.empty()) {
            module.getContext().diagnose(TypeRequiredError("a parameter of function", fn->getName().str()));
        }
        if (annotation.Name.empty()) {
            module.getContext().diagnose(NameRequiredError("a parameter of function", fn->getName().str()));
        }
    }

    return annotations;
}

auto polyglat::ir::ParameterAnnotation::ToString() const -> std::string {
    return std::format("<param type=\"{}\" name=\"{}\"/>", Type, Name);
}

auto polyglat::ir::FunctionAnnotation::Create(llvm::Module &module, llvm::Function *fn, const std::vector<llvm::StringRef> &annotations) -> std::optional<FunctionAnnotation> {
    auto ret = 0U;

    std::string ns, name, callconv, retT;
    for (const auto &annotation : annotations) {
        auto parsed = shared::ParseAnnotation(annotation.str());
        if (!parsed) {
            continue;
        }

        if (auto [key, value] = parsed.value(); key == shared::NAMESPACE) {
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
            module.getContext().diagnose(UnrecognizedAttributeError(key, "of a parameter of function", fn->getName().str()));
        }
    }

    if ((ret & 1U) == 0) {
        module.getContext().diagnose(NamespaceRequiredError("function", fn->getName().str()));
    }
    if ((ret & 1U << 1U) == 0) {
        module.getContext().diagnose(NameRequiredError("function", fn->getName().str()));
    }
    if ((ret & 1U << 2U) == 0) {
        module.getContext().diagnose(CallConvRequiredError("function", fn->getName().str()));
    }
    if ((ret & 1U << 3U) == 0) {
        module.getContext().diagnose(ReturnTypeRequiredError("function", fn->getName().str()));
    }
    if (constexpr unsigned ALL = 0b1111; ret != ALL) {
        return std::nullopt;
    }

    const auto params = ParameterAnnotation::Create(module, fn);

    return FunctionAnnotation{fn, ns, name, callconv, retT, params};
}

auto polyglat::ir::FunctionAnnotation::CreateVector(llvm::Module &module, const std::vector<RawFunctionAnnotation> &annotations) -> std::vector<FunctionAnnotation> {

    std::map<llvm::Function *, std::vector<llvm::StringRef>> map;
    for (auto annotation : annotations) {
        map[annotation.getFunction()].push_back(annotation.getValue());
    }

    std::vector<FunctionAnnotation> ret;
    for (const auto &[fn, annotations] : map) {
        if (auto annotation = Create(module, fn, annotations)) {
            ret.push_back(annotation.value());
        }
    }

    return ret;
}

auto polyglat::ir::FunctionAnnotation::ToString() const -> std::string {
    std::ostringstream os;
    os << "<function ns=\"" << Namespace << "\" name=\"" << Name << "\" callconv=\"" << CallConv << "\" return=\"" << Return << '"';

    if (Parameters.empty()) {
        os << "/>";
    } else {
        os << '>';
        for (const auto &annotation : Parameters) {
            os << annotation.ToString();
        }
        os << "</function>";
    }

    return os.str();
}
