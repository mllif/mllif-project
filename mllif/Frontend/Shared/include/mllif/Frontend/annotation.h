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

#include <string>
#include <vector>

namespace clang {
    class FunctionDecl;
} // namespace clang

namespace mllif::shared {
    constexpr std::string Namespace = "$mllif";

    namespace prefix {
        constexpr std::string Path = "path";
        constexpr std::string Type = "type";
    } // namespace prefix

    namespace type {
        constexpr std::string Function = "function";
        constexpr std::string Method = "method";
        constexpr std::string Object = "object";
    } // namespace type

    struct Annotation {
        std::string Key;
        std::vector<std::string> Values;

        Annotation(const std::string &annotation);
    };

    /**
     * @brief Annotate a declaration with its information that may be lost
     * @param decl A declaration to be annotated
     */
    void CreateAnnotation(clang::FunctionDecl *decl);


} // namespace mllif::shared
