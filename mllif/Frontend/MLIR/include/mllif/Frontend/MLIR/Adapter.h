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

#include <mllif/Frontend/MLIR/Tree.h>

namespace mllif::mlir {

    /**
     * @brief An adapter for each dialect in module. one adapter should match to one dialect, and vice versa. (one-to-one)
     */
    class Adapter {
      public:
        virtual ~Adapter() = default;

        virtual void handle(Tree &symbols, std::shared_ptr<::mlir::ModuleOp> module, ::mlir::Operation *op) = 0;
    };

    extern std::vector<std::shared_ptr<Adapter>> Adapters;

    template <typename T>
    class AdapterRegistry {
      public:
        AdapterRegistry() {
            Adapters.push_back(std::make_shared<T>());
        }
    };

} // namespace mllif::mlir
