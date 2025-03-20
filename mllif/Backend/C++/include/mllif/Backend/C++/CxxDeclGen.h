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

#include <mllif/Backend/C++/CxxBridgeGen.h>

namespace mllif::cxx {
    class CxxDeclGen final : public CxxBridgeGen {
      public:
        bool handleAssemblyBegin(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleObjectBegin(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) override;
    };
}
