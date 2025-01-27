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
#define NESTED_DECL(t, cond, msg, I, ...)                             \
    if (const auto decl = std::dynamic_pointer_cast<t##Decl>(node)) { \
        if (!handle##t##Begin(context, *decl, out, indent)) {         \
            return false;                                             \
        }                                                             \
                                                                      \
        for (auto i = 0; i < decl->children().size(); ++i) {          \
            auto &child = decl->children()[i];                        \
            if (cond) {                                               \
                context.error(msg);                                   \
                break;                                                \
            }                                                         \
            if (!handleDecl(context, child, out, indent + 1 + I)) {   \
                break;                                                \
            }                                                         \
            __VA_ARGS__                                               \
        }                                                             \
                                                                      \
        if (!handle##t##End(context, *decl, out, indent)) {           \
            return false;                                             \
        }                                                             \
    }

    NESTED_DECL(Assembly, false, "", -1)
    else NESTED_DECL(Namespace, CHILD_OF(Assembly), "AssemblyDecl cannot be the child of NamespaceDecl", 0)
    else NESTED_DECL(Object, CHILD_OF(Namespace) || CHILD_OF(Assembly), "AssemblyDecl or NamespaceDecl cannot be the child of ObjectDecl", 0)
    else NESTED_DECL(
        Method, !CHILD_OF(Param),
        "Only ParamDecl can be the child of MethodDecl", 0,
        if (i != decl->children().size() - 1) { writeParamDelimiter(out); })
    else NESTED_DECL(
        Function, !CHILD_OF(Param),
        "Only ParamDecl can be the child of FunctionDecl", 0,
        if (i != decl->children().size() - 1) { writeParamDelimiter(out); })
    else if (const auto decl = std::dynamic_pointer_cast<ParamDecl>(node)) {
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