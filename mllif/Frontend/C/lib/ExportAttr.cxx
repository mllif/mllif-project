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

#include "pch.h"
#include "mllif/Frontend/annotation.h"
#include "mllif/Frontend/C/ExportAttr.h"

mllif::c::ExportAttrInfo::ExportAttrInfo() {
    // NOTE: `S` should be `static` (experimental proof on Clang/LLVM 20.0)
    static constexpr std::array<Spelling, 3> S = {{
        {
            .Syntax = clang::ParsedAttr::AS_GNU,
            .NormalizedFullName = "mllif_export",
        },
        {
            .Syntax = clang::ParsedAttr::AS_C23,
            .NormalizedFullName = "mllif::export",
        },
        {
            .Syntax = clang::ParsedAttr::AS_CXX11,
            .NormalizedFullName = "mllif::export",
        },
    }};
    Spellings = S;
}

auto mllif::c::ExportAttrInfo::diagAppertainsToDecl(clang::Sema &/**/, const clang::ParsedAttr &/**/, const clang::Decl *D) const -> bool {
    return clang::dyn_cast<clang::FunctionDecl>(D) || clang::dyn_cast<clang::CXXRecordDecl>(D);
}

auto mllif::c::ExportAttrInfo::diagAppertainsToStmt(clang::Sema &/**/, const clang::ParsedAttr &/**/, const clang::Stmt */**/) const -> bool {
    return false;
}

auto mllif::c::ExportAttrInfo::handleDeclAttribute(clang::Sema &S, clang::Decl *D, const clang::ParsedAttr &/**/) const -> AttrHandling {
    if (const auto record = clang::dyn_cast<clang::CXXRecordDecl>(D)) {
        for (const auto method : record->methods()) {
            if (method->getVisibility() != clang::Visibility::HiddenVisibility) {
                shared::CreateAnnotation(method);
            }
        }
    } else if (const auto fn = clang::dyn_cast<clang::FunctionDecl>(D)) {
        shared::CreateAnnotation(fn); // just mark a symbol to process it later
    } else {
        const auto id = S.Diags.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "%0 '%1' can not be exported; Only function can be exported. "
            "If you want to export records such as class or struct, You can make a function that depends on these records");

        std::string name;
        if (const auto named = clang::dyn_cast<clang::NamedDecl>(D)) {
            name = named->getName();
        } else {
            name = std::to_string(D->getID());
        }

        S.Diag(D->getLocation(), id) << D->getDeclKindName() << name;

        return AttributeNotApplied;
    }

    return AttributeApplied;
}

namespace {
    [[maybe_unused]]
    //NOLINTNEXTLINE(cert-err58-cpp) : Initialization of 'Y' with static storage duration may throw an exception that cannot be caught
    const clang::ParsedAttrInfoRegistry::Add<mllif::c::ExportAttrInfo> Y("mllif", "");
}
