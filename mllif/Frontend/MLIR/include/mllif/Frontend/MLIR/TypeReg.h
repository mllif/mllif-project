#pragma once

#include "mllif/Frontend/MLIR/Type.h"

// Include all type implementations
#include "mllif/Frontend/MLIR/Builtin/BuiltinType.h"
#include "mllif/Frontend/MLIR/CIR/CIRType.h"

namespace mllif::mlir {

    template<typename TInit, typename ...T>
    class TypeReg {
    public:
        static auto From(const ::mlir::Type &type, std::shared_ptr<::mlir::ModuleOp> module) -> std::shared_ptr<Type> {
            if (auto t = TInit::From(type, module)) {
                return t;
            }
            if constexpr (sizeof...(T) > 0) {
                return TypeReg<T...>::From(type, module);
            }
            return nullptr;
        }
    };

    // Ignore IDE error - Compilation may success
    using Types = TypeReg<
#include "mllif/Frontend/MLIR/Types.inc"
        >;
}