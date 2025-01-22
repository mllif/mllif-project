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

#include <memory>
#include <mllif/Frontend/MLIR/Type.h>

namespace mlir {
    class Type;
}

namespace mllif::mlir::cir {

    /**
     * @brief Wrapper class for types in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRType : public Type {

      protected:
        CIRType() = default;

      public:
        static auto From(const ::mlir::Type &type, const std::shared_ptr<::mlir::ModuleOp> &module) -> std::shared_ptr<CIRType>;
    };

    /**
     * @brief Wrapper class for boolean in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRBoolType final : public CIRType {
      public:
        CIRBoolType() = default;

        std::string store(Tree &symbols) const override;
    };

    /**
     * @brief Wrapper class for integer types in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRIntegerType final : public CIRType {
        size_t _width;
        bool _signed;

      public:
        CIRIntegerType(size_t width, bool _signed) : _width(width), _signed(_signed) {}

        size_t &width() { return _width; }
        size_t width() const { return _width; }
        bool &signed_() { return _signed; }
        bool signed_() const { return _signed; }

        std::string store(Tree &symbols) const override;
    };

    /**
     * @brief Wrapper class for floating-point types in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRFloatType final : public CIRType {
        size_t _width;

      public:
        CIRFloatType(const size_t width) : _width(width) {}

        size_t &width() { return _width; }
        size_t width() const { return _width; }

        std::string store(Tree &symbols) const override;
    };

    /**
     * @brief Wrapper class for pointer in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRPointerType final : public CIRType {
        std::shared_ptr<CIRType> _pointee;

      public:
        explicit CIRPointerType(const std::shared_ptr<CIRType> &type) : _pointee(type) {}

        std::shared_ptr<CIRType> &pointee() { return _pointee; }
        const std::shared_ptr<CIRType> &pointee() const { return _pointee; }

        std::string store(Tree &symbols) const override;
    };

    /**
     * @brief Wrapper class for user-defined structs in CIR dialect to serialize it to simple-and-plain text
     */
    class CIRStructType final : public CIRType {
        std::deque<std::string> _path;
        size_t _size;
        size_t _align;

      public:
        CIRStructType(const std::deque<std::string> &path, const size_t size, const size_t align) : _path(path), _size(size), _align(align) {}

        std::deque<std::string> &path() { return _path; }
        const std::deque<std::string> &path() const { return _path; }
        size_t size() { return _size; }
        size_t size() const { return _size; }
        size_t align() { return _align; }
        size_t align() const { return _align; }

        std::string store(Tree &symbols) const override;
    };

} // namespace mllif::mlir::cir
