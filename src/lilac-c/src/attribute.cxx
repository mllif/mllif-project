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
#include "lilac-c/attribute.h"

#include <stack>
#include <sstream>

using namespace clang;

std::string lilac::c::Attributes::GetFullName(NamedDecl* decl)
{
    std::stack<std::string> stk;
    do
        stk.emplace(decl->getName());
    while ((decl = clang::dyn_cast<NamedDecl>(decl->getDeclContext())));

    std::stringstream ss;
    for (; !stk.empty(); stk.pop())
        ss << '/' << stk.top();

    return ss.str();
}

void lilac::c::Attributes::Annotate(NamedDecl* namedDecl, const ParsedAttr& Attr)
{
    if (clang::dyn_cast<TypeDecl>(namedDecl))
    {
        const auto annotation = AnnotateTypeAttr::Create(
            namedDecl->getASTContext(),
            NS + GetFullName(namedDecl),
            nullptr, 0,
            Attr.getRange());

        namedDecl->addAttr(annotation);

        return;
    }

    AnnotateAttr* annotation;

    if (clang::dyn_cast<ParmVarDecl>(namedDecl))
    {
        annotation = AnnotateAttr::Create(
            namedDecl->getASTContext(),
            NS + namedDecl->getNameAsString(),
            nullptr, 0,
            Attr.getRange());
    }
    else
    {
        annotation = AnnotateAttr::Create(
            namedDecl->getASTContext(),
            NS + GetFullName(namedDecl),
            nullptr, 0,
            Attr.getRange());
    }

    namedDecl->addAttr(annotation);
}

namespace
{
    class ExportAttrInfo final : public ParsedAttrInfo
    {
    public:
        ExportAttrInfo()
        {
            // NOTE: `S` should be `static` (experimental proof on Clang/LLVM 20.0)
            static constexpr Spelling S[] = {
                { ParsedAttr::AS_GNU, "lilac_export" },
                { ParsedAttr::AS_C23, "lilac::export" },
                { ParsedAttr::AS_CXX11, "lilac::export" }
            };
            Spellings = S;
        }

        bool diagAppertainsToDecl(Sema& S, const ParsedAttr& Attr, const Decl* D) const override
        {
            return clang::dyn_cast<RecordDecl>(D) || clang::dyn_cast<FunctionDecl>(D);
        }

        bool diagAppertainsToStmt(Sema&, const ParsedAttr&, const Stmt*) const override
        {
            return false;
        }

        AttrHandling handleDeclAttribute(Sema& S, Decl* D, const ParsedAttr& Attr) const override
        {
            if (const auto record = clang::dyn_cast<RecordDecl>(D))
                lilac::c::Attributes::Annotate(record, Attr);
            else if (const auto fn = clang::dyn_cast<FunctionDecl>(D))
            {
                lilac::c::Attributes::Annotate(fn, Attr);
                for (const auto parameter : fn->parameters())
                    lilac::c::Attributes::Annotate(parameter, Attr);
            }
            else
            {
                const auto id = S.Diags.getCustomDiagID(
                    DiagnosticsEngine::Warning,
                    "%0 '%1' can not be exported; Only record and function can be exported.");

                std::string name;
                if (const auto named = clang::dyn_cast<NamedDecl>(D))
                    name = named->getName();
                else
                    name = std::to_string(D->getID());

                S.Diag(D->getLocation(), id) << D->getDeclKindName() << name;

                return AttributeNotApplied;
            }

            return AttributeApplied;
        }
    };
}

[[maybe_unused]]
static ParsedAttrInfoRegistry::Add<ExportAttrInfo> Y("lilac","");
