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

#pragma once

#include <mllif/Frontend/MLIR/Type.h>

// Include all type implementations
#include <mllif/Frontend/MLIR/Builtin/BuiltinType.h>
#include <mllif/Frontend/MLIR/CIR/CIRType.h>

namespace mllif::mlir {

    template <typename TInit, typename... T>
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
#include <mllif/Frontend/MLIR/Types.inc>
        >;
} // namespace mllif::mlir