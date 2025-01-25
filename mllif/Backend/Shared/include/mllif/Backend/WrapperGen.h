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

#include "Decl.h"

#include <ostream>

namespace mllif {
    class Decl;
    class AssemblyDecl;
    class NamespaceDecl;
    class ObjectDecl;
    class MethodDecl;
    class FunctionDecl;
    class ParamDecl;
}

namespace mllif {

    class WrapperGen {
      public:
        virtual ~WrapperGen() = default;

#define MLLIF_GEN_HANDLER(type,...) virtual bool handle##type##__VA_ARGS__ (MLLIFContext& context, const type##Decl &node, std::ostream& out, std::size_t indent)

        MLLIF_GEN_HANDLER(Assembly, Begin) = 0;
        MLLIF_GEN_HANDLER(Assembly, End) = 0;
        MLLIF_GEN_HANDLER(Namespace, Begin) = 0;
        MLLIF_GEN_HANDLER(Namespace, End) = 0;
        MLLIF_GEN_HANDLER(Object, Begin) = 0;
        MLLIF_GEN_HANDLER(Object, End) = 0;
        MLLIF_GEN_HANDLER(Function, Begin) = 0;
        MLLIF_GEN_HANDLER(Function, End) = 0;
        MLLIF_GEN_HANDLER(Method, Begin) = 0;
        MLLIF_GEN_HANDLER(Method, End) { return handleFunctionEnd(context, node, out, indent); }
        MLLIF_GEN_HANDLER(Param) = 0;

#undef MLLIF_GEN_HANDLER

        bool handleDecl(MLLIFContext& context, const std::shared_ptr<Decl> &node, std::ostream& out, std::size_t indent);
    };

}
