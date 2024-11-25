#pragma once
#include <lilac-shared/annotation.h>

namespace lilac::ir {

class RawFunctionAnnotation {
    llvm::Function *Function;
    llvm::StringRef Value;

    RawFunctionAnnotation(llvm::Function *function, const llvm::StringRef value) : Function(function), Value(value) {}

  public:
    static auto Create(llvm::Module &module, const llvm::ConstantStruct *constantStruct) -> std::optional<RawFunctionAnnotation>;

    static auto CreateVector(llvm::Module &module) -> std::vector<RawFunctionAnnotation>;

    [[nodiscard]] auto getFunction() const -> llvm::Function * { return Function; }
    [[nodiscard]] auto getValue() const -> llvm::StringRef { return Value; }
};

class ParameterAnnotation {
    std::string Type;
    std::string Name;

    ParameterAnnotation(std::string type, std::string name) : Type(std::move(type)), Name(std::move(name)) {}

    static auto ParseCallInst(llvm::Module &module, llvm::Function *fn, llvm::CallInst *call) -> std::optional<std::pair<std::string, std::string>>;

  public:
    static auto Create(llvm::Module &module, llvm::Function *fn) -> std::optional<ParameterAnnotation>;
};

class FunctionAnnotation {
    llvm::Function *Function;
    std::string Namespace;
    std::string Name;
    std::string CallConv;
    std::string Return;
    std::vector<ParameterAnnotation> Parameters;

    FunctionAnnotation(
        llvm::Function *function,
        std::string ns,
        std::string name,
        std::string callConv,
        std::string returnValue)
        : Function(function),
          Namespace(std::move(ns)),
          Name(std::move(name)),
          CallConv(std::move(callConv)),
          Return(std::move(returnValue)) {}

  public:
    static auto Create(llvm::Function *function, const std::vector<llvm::StringRef> &annotations) -> std::optional<FunctionAnnotation>;

    static auto CreateVector(const std::vector<RawFunctionAnnotation> &annotations) -> std::vector<FunctionAnnotation>;

    [[nodiscard]] auto getFunction() const -> llvm::Function * { return Function; }
    [[nodiscard]] auto getNamespace() const -> const std::string& { return Namespace; }
    [[nodiscard]] auto getName() const -> const std::string& { return Name; }
    [[nodiscard]] auto getCallConv() const -> const std::string& { return CallConv; }
    [[nodiscard]] auto getReturnType() const -> const std::string& { return Return; }
    [[nodiscard]] auto getParameters() const -> const std::vector<ParameterAnnotation>& { return Parameters; }
};
} // namespace lilac::ir
