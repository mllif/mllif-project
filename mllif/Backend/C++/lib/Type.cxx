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

#include <mllif/Backend/C++/Type.h>

extern std::ostream& errs();

std::string mllif::cxx::AnnotationToCxxType(std::string annotation) {
    auto ref = 0;
    while (annotation.ends_with('*')) {
        annotation.erase(annotation.size() - 1);
        ref++;
    }

    if (annotation.starts_with('/')) {
        for (size_t p; (p = annotation.find('/')) != std::string::npos; ) {
            annotation.replace(p, 1, "::");
        }
    } else /* built-ins */ {
        static std::map<std::string, std::string> map = {
#define T_INT(bit) { "s" #bit , "std::int" #bit "_t" }, { "u" #bit , "std::uint" #bit "_t" }
            T_INT(8),
            T_INT(16),
            T_INT(32),
            T_INT(64),
            { "s128", "__int128_t" },
            { "u128", "__uint128_t" },
            { "fp16", "_Float16" },
            { "fp32", "float" },
            { "fp64", "double" },
            { "fp128", "__float128" },
            { "bool", "bool" },
            { "void", "void" }
#undef T_INT
        };

        if (!map.contains(annotation)) {
            errs() << "unknown type '" << annotation << "'" << std::endl;
            return "<UNKNOWN>";
        }

        annotation = map[annotation];
    }

    for (auto i = 0; i < ref; i++) {
        annotation += '*';
    }

    return annotation;
}
