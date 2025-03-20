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

#include <mllif/Backend/Decl.h>
#include <string>

namespace mllif::cs {

    /**
     * @brief Gets C#-compliant typename of type
     * @param type Type to convert
     * @return C++-compliant typename
     * @retval `std::nullopt` if type represents unrecognized type
     */
    std::optional<std::string> TypeToCs(const Type &type);

} // namespace mllif::cs
