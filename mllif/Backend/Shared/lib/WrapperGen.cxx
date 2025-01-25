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

#include <mllif/Backend/Context.h>
#include <mllif/Backend/WrapperGen.h>

bool mllif::WrapperGen::handleDecl(MLLIFContext &context, const std::shared_ptr<Decl> &node, std::ostream &out, std::size_t indent) {
#define CHILD_OF(t) std::dynamic_pointer_cast<t##Decl>(child)
#define NESTED_DECL(t, cond, msg)                                     \
    if (const auto decl = std::dynamic_pointer_cast<t##Decl>(node)) { \
        if (!handle##t##Begin(context, *decl, out, indent)) {         \
            return false;                                             \
        }                                                             \
                                                                      \
        for (auto child : decl->children()) {                         \
            if (cond) {                                               \
                context.error(msg);                                   \
                break;                                                \
            }                                                         \
            if (!handleDecl(context, child, out, indent + 1)) {       \
                break;                                                \
            }                                                         \
        }                                                             \
                                                                      \
        if (!handle##t##End(context, *decl, out, indent + 1)) {       \
            return false;                                             \
        }                                                             \
    }

    NESTED_DECL(Assembly, false, "")
    NESTED_DECL(Namespace, CHILD_OF(Assembly), "AssemblyDecl cannot be the child of NamespaceDecl")
    NESTED_DECL(Object, CHILD_OF(Namespace) || CHILD_OF(Assembly), "AssemblyDecl or NamespaceDecl cannot be the child of ObjectDecl")
    NESTED_DECL(Function, !CHILD_OF(Param), "Only ParamDecl can be the child of FunctionDecl")
    NESTED_DECL(Method, !CHILD_OF(Param), "Only ParamDecl can be the child of MethodDecl")

    if (const auto decl = std::dynamic_pointer_cast<ParamDecl>(node)) {
        if (!handleParam(context, *decl, out, indent)) {
            return false;
        }
        if (decl->children().size()) {
            context.error("ParamDecl cannot have any children");
            return false;
        }
    }

    return true;
}