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

namespace mllif::mlir::builtin {

    /**
     * @brief Wrapper class for types in builtin MLIR dialect to serialize it to simple-and-plain text
     */
    class BuiltinType : public Type {
      public:
        static auto From(const ::mlir::Type &type, const std::shared_ptr<::mlir::ModuleOp>& module) -> std::shared_ptr<BuiltinType>;
    };

    /**
     * @brief Wrapper class for integer types in builtin MLIR dialect to serialize it to simple-and-plain text
     */
    class BuiltinIntType final : public BuiltinType {
        size_t _width;
        bool _signed;

      public:
        BuiltinIntType(size_t width, bool _signed) : _width(width), _signed(_signed) {}

        /**
         * @brief Gets bit-width of integer
         * @return Bit-width of integer
         */
        size_t &width() { return _width; }

        /**
         * @brief Gets bit-width of integer
         * @return Bit-width of integer
         */
        size_t width() const { return _width; }

        /**
         * @brief Gets signedness of integer
         * @retval true if an integer is signed integer
         * @retval false if an integer is unsigned integer
         */
        bool &signed_() { return _signed; }

        /**
         * @brief Gets signedness of integer
         * @retval true if an integer is signed integer
         * @retval false if an integer is unsigned integer
         */
        bool signed_() const { return _signed; }

        std::string store(Tree &symbols) const override;
    };

    /**
     * @brief Wrapper class for floating-point types in builtin MLIR dialect to serialize it to simple-and-plain text
     */
    class BuiltinFPType final : public BuiltinType {
        size_t _width;

      public:
        BuiltinFPType(size_t width) : _width(width) {}

        /**
         * @brief Gets bit-width of float
         * @return Bit-width of float
         */
        size_t &width() { return _width; }

        /**
         * @brief Gets bit-width of float
         * @return Bit-width of float
         */
        size_t width() const { return _width; }

        std::string store(Tree &symbols) const override;
    };

} // namespace mllif::mlir::builtin
