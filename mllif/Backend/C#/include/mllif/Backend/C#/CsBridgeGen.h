/*
 * Copyright  Yeong-won Seo
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

#include <mllif/Backend/BridgeGen.h>

namespace mllif::cs {
    class CsBridgeGen final : public BridgeGen {
      public:
        std::string LibraryName;

        CsBridgeGen(const std::string& libname);

        bool handleAssemblyBegin(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleAssemblyEnd(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleNamespaceBegin(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleNamespaceEnd(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleObjectBegin(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleObjectEnd(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleFunctionBegin(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleFunctionEnd(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleMethodBegin(MLLIFContext &context, const MethodDecl &node, std::ostream &out, std::size_t indent) override;
        bool handleParam(MLLIFContext &context, const ParamDecl &node, std::ostream &out, std::size_t indent) override;
        void writeParamDelimiter(std::ostream &os) override;
    };
} // namespace mllif::cs
