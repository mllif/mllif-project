/*
 * Copyright 2024 Yeong-won Seo
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

#include <string>
#include <clang/AST/Attr.h>
#include <clang/Sema/ParsedAttr.h>

namespace lilac::c
{
    struct Attributes
    {
        static constexpr std::string NS = "__lilac__";

        static std::string GetFullName(clang::NamedDecl* decl);

        static void Annotate(clang::NamedDecl* namedDecl, const clang::ParsedAttr& Attr);
    };
}
