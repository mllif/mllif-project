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

#include "lilac-c/pch.h"

#include <lilac-shared/annotation.h>

namespace {
    class ExportAttrInfo final : public clang::ParsedAttrInfo {
      public:
        ExportAttrInfo() {
            // NOTE: `S` should be `static` (experimental proof on Clang/LLVM 20.0)
            static constexpr std::array<Spelling, 3> S = {{
                {
                    .Syntax = clang::ParsedAttr::AS_GNU,
                    .NormalizedFullName = "lilac_export",
                },
                {
                    .Syntax = clang::ParsedAttr::AS_C23,
                    .NormalizedFullName = "lilac::export",
                },
                {
                    .Syntax = clang::ParsedAttr::AS_CXX11,
                    .NormalizedFullName = "lilac::export",
                },
            }};
            Spellings = S;
        }

        auto diagAppertainsToDecl(clang::Sema & /**/, const clang::ParsedAttr & /**/, const clang::Decl *D) const -> bool override {
            return clang::dyn_cast<clang::RecordDecl>(D) || clang::dyn_cast<clang::FunctionDecl>(D);
        }

        auto diagAppertainsToStmt(clang::Sema & /**/, const clang::ParsedAttr & /**/, const clang::Stmt * /**/) const -> bool override {
            return false;
        }

        auto handleDeclAttribute(clang::Sema &S, clang::Decl *D, const clang::ParsedAttr &/**/) const -> AttrHandling override {
            if (const auto record = clang::dyn_cast<clang::RecordDecl>(D)) {
                lilac::shared::MarkAsTarget(record);
            } else if (const auto fn = clang::dyn_cast<clang::FunctionDecl>(D)) {
                lilac::shared::MarkAsTarget(fn);
            } else {
                const auto id = S.Diags.getCustomDiagID(
                    clang::DiagnosticsEngine::Warning,
                    "%0 '%1' can not be exported; Only record and function can be exported.");

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
    };
} // namespace

namespace {
    [[maybe_unused]]
    //NOLINTNEXTLINE(cert-err58-cpp) : Initialization of 'Y' with static storage duration may throw an exception that cannot be caught
    const clang::ParsedAttrInfoRegistry::Add<ExportAttrInfo> Y("lilac", "");
}
