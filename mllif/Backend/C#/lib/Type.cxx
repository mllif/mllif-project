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

#include <mllif/Backend/C#/Type.h>

std::optional<std::string> mllif::cs::TypeToCs(const Type &type) {
    std::stringstream ss;

    if (type.builtin()) {
        static std::map<std::string, std::string> map = {
#define T_INT(bit) { "s" #bit , "System.Int" #bit }, { "u" #bit , "System.UInt" #bit }
            { "s8", "System.SByte" },
            { "u8", "System.Byte" },
            T_INT(16),
            T_INT(32),
            T_INT(64),
            { "s128", "System.Int128" },
            { "u128", "System.UInt128" },
            { "fp16", "System.Half" },
            { "fp32", "float" },
            { "fp64", "double" },
            // { "fp128", "" },  <-- C# doesn't support binary128 of IEEE754
            { "bool", "bool" },
            { "void", "void" }
#undef T_INT
        };

        auto t = type.terms()[0];
        if (!map.contains(t)) {
            return std::nullopt;
        }

        ss << map[t];
    } else {
        for (auto i = 0; i < type.terms().size(); ++i) {
            if (i != 0) {
                ss << '.';
            }

            ss << type.terms()[i];
        }
    }

    ss << std::string(type.refs(), '*');

    return ss.str();
}
