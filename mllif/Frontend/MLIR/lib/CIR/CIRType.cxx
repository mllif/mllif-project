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

#include <mllif/Frontend/MLIR/CIR/CIRType.h>
#include <mllif/Frontend/annotation.h>

namespace {
    std::deque<std::string> Split(std::string s, const std::string &del) {
        size_t start = 0, end;
        const size_t del_len = del.length();
        std::deque<std::string> res;

        while ((end = s.find(del, start)) != std::string::npos) {
            auto token = s.substr(start, end - start);
            res.push_back(token);

            start = end + del_len;
        }

        res.push_back(s.substr(start));
        return res;
    }
} // namespace

auto mllif::mlir::cir::CIRType::From(const ::mlir::Type &type, std::shared_ptr<::mlir::ModuleOp> module) -> std::shared_ptr<CIRType> {
    if (dyn_cast<::cir::BoolType>(type)) {
        return std::make_shared<CIRBoolType>();
    }

    if (const auto i = dyn_cast<::cir::IntType>(type)) {
        return std::make_shared<CIRIntegerType>(
            i.getWidth(),
            !type.isUnsignedInteger() // Regard signless as signed
        );
    }

    if (dyn_cast<::cir::FP16Type>(type)) {
        return std::make_shared<CIRFloatType>(16);
    }
    if (dyn_cast<::cir::SingleType>(type)) {
        return std::make_shared<CIRFloatType>(32);
    }
    if (dyn_cast<::cir::DoubleType>(type)) {
        return std::make_shared<CIRFloatType>(64);
    }
    if (dyn_cast<::cir::FP128Type>(type)) {
        return std::make_shared<CIRFloatType>(128);
    }

    if (const auto pointer = dyn_cast<::cir::PointerType>(type)) {
        return std::make_shared<CIRPointerType>(From(pointer.getPointee(), module));
    }

    if (const auto _struct = dyn_cast<::cir::StructType>(type)) {
        const ::mlir::DataLayout layout{module->clone()};
        const auto params = module->getDataLayoutSpec().getEntries();

        const auto size = _struct.getTypeSize(layout, module->getDataLayoutSpec().getEntries());
        const auto align = _struct.getABIAlignment(layout, params);

        const auto path = Split(_struct.getName().str(), "::");

        return std::make_shared<CIRStructType>(path, size, align);
    }

    return nullptr;
}

std::string mllif::mlir::cir::CIRBoolType::store(Tree &symbols) const {
    return "bool";
}

std::string mllif::mlir::cir::CIRIntegerType::store(Tree &symbols) const {
    std::stringstream ss;
    ss << (signed_() ? 's' : 'u') << width();
    return ss.str();
}

std::string mllif::mlir::cir::CIRFloatType::store(Tree &symbols) const {
    std::stringstream ss;
    ss << "fp" << width();
    return ss.str();
}

std::string mllif::mlir::cir::CIRPointerType::store(Tree &symbols) const {
    std::stringstream ss;
    ss << pointee()->store(symbols) << '*';
    return ss.str();
}

std::string mllif::mlir::cir::CIRStructType::store(Tree &symbols) const {
    auto copyPath = path();
    auto symbol = symbols.root().insert_inplace(copyPath, shared::type::Object);

    bool sizeSet = false, alignSet = false;
    for (auto &[key, value] : symbol->attributes()) {
        if (key == "size") {
            sizeSet = true;
        } else if (key == "align") {
            alignSet = true;
        }
    }
    if (!sizeSet) {
        symbol->attributes().push_back(std::make_pair("size", std::to_string(size())));
    }
    if (!alignSet) {
        symbol->attributes().push_back(std::make_pair("align", std::to_string(align())));
    }

    std::stringstream ss;
    for (const auto &term : path()) {
        ss << '/' << term;
    }
    return ss.str();
}
