#include "pch.h"
#include "mllif/Frontend/MLIR/Builtin/BuiltinType.h"

auto mllif::mlir::builtin::BuiltinType::From(const ::mlir::Type &type, std::shared_ptr<::mlir::ModuleOp> /**/) -> std::shared_ptr<BuiltinType> {
    if (const auto i = dyn_cast<::mlir::IntegerType>(type)) {

        auto sign = i.getSignedness();
        if (sign == ::mlir::IntegerType::Signless) {
            // Signedness defaults to signed
            sign = ::mlir::IntegerType::Signed;
        }

        return std::make_shared<BuiltinIntType>(
            i.getWidth(),
            sign == ::mlir::IntegerType::Signed);
    }
    if (type.isF16()) {
        return std::make_shared<BuiltinFPType>(16);
    }
    if (type.isF32()) {
        return std::make_shared<BuiltinFPType>(32);
    }
    if (type.isF64()) {
        return std::make_shared<BuiltinFPType>(64);
    }
    if (type.isF128()) {
        return std::make_shared<BuiltinFPType>(128);
    }

    return nullptr;
}

std::string mllif::mlir::builtin::BuiltinIntType::store(Tree &symbols) const {
    std::stringstream ss;
    ss << (signed_() ? 's' : 'u') << width();
    return ss.str();
}

std::string mllif::mlir::builtin::BuiltinFPType::store(Tree &symbols) const {
    std::stringstream ss;
    ss << "fp" << width();
    return ss.str();
}
