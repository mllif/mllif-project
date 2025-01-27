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

#include "pch.h"

#include <memory>
#include <mllif/Backend/C++/CxxDeclGen.h>
#include <mllif/Backend/C++/CxxWrapperGen.h>
#include <mllif/Backend/C++/Type.h>
#include <mllif/Backend/Context.h>
#include <mllif/Backend/Decl.h>
#include <mllif/Backend/Stdin.h>

static void PrintErrors(const mllif::MLLIFContext &context) {
    for (auto error : context.errors()) {
        std::cerr << "\x1b[0;31merror\x1b[0m: " << error.what() << std::endl;
    }
}

auto main() -> int {
    auto msm = mllif::Stdin::ReadToEnd();

    rapidxml::xml_document<> doc;
    doc.parse<0>(msm.data());

    mllif::MLLIFContext context;
    const auto root = mllif::Decl::Create(context, doc.first_node(), nullptr);

    if (!context) {
        goto ERROR;
    }

    {
        mllif::cxx::CxxDeclGen declGen;
        declGen.handleDecl(context, root, std::cout, 0);

        if (!context) {
            goto ERROR;
        }
    }

    {
        mllif::cxx::CxxWrapperGen wrapperGen;
        wrapperGen.handleDecl(context, root, std::cout, 0);

        if (!context) {
            goto ERROR;
        }
    }

    return EXIT_SUCCESS;

ERROR:
    PrintErrors(context);
    return EXIT_FAILURE;
}
