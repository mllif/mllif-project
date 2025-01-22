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
#include "Tree.h"

namespace mllif::mlir {

    /**
     * @brief A wrapper class of `::mlir::Type` that serializes it into simple-and-plain text
     */
    class Type {
      public:
        virtual ~Type() = default;

        /**
         * @brief Stores this type into symbol tree if necessary and Returns reference string for this type
         * @param symbols A symbol tree
         * @return Reference string for this type
         */
        virtual std::string store(Tree &symbols) const = 0;
    };

} // namespace mllif::mlir
