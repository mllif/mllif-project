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

#include <mllif/Frontend/MLIR/Builtin/BuiltinType.h>

auto mllif::mlir::builtin::BuiltinType::From(const ::mlir::Type &type, const std::shared_ptr<::mlir::ModuleOp>& /**/) -> std::shared_ptr<BuiltinType> {
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

std::string mllif::mlir::builtin::BuiltinIntType::store(Tree &/**/) const {
    std::stringstream ss;
    ss << (signed_() ? 's' : 'u') << width();
    return ss.str();
}

std::string mllif::mlir::builtin::BuiltinFPType::store(Tree &/**/) const {
    std::stringstream ss;
    ss << "fp" << width();
    return ss.str();
}
