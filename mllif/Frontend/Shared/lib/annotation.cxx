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

#include <mllif/Frontend/annotation.h>
#include <llvm/Support/CommandLine.h>

namespace {

    constexpr char Delimiter = '\\';

    /*
    clang::ConstantExpr *CreateExpr(const clang::ASTContext& ctx, const std::string &str) {
        const auto arrayTy =
            ctx.getConstantArrayType(
                   ctx.CharTy,
                   llvm::APInt(ctx.getTypeSize(ctx.getSizeType()), str.size() + 1),
                   nullptr,
                   clang::ArraySizeModifier::Normal,
                   0)
                .withConst();

        const auto literal = clang::StringLiteral::Create(
                ctx,
                str,
                clang::StringLiteralKind::Ordinary,
                false,
                arrayTy,
                {});

        return clang::ConstantExpr::Create(ctx, literal);
    }
    */

    void ApplyAnnotation(clang::NamedDecl *decl, const std::string &key, const std::string* values, const size_t valuesSize) {
        auto &ctx = decl->getASTContext();

        /*
        std::vector<clang::Expr *> args;
        args.reserve(valuesSize);
        for (auto i = 0; i < valuesSize; i++) {
            args.push_back(CreateExpr(ctx, values[i]));
        }

        // Same size of args makes args copied... Why???
        const auto attr = clang::AnnotateAttr::CreateImplicit(ctx, mllif::shared::Namespace + '.' + key, args.data(), args.size());
        */

        std::stringstream ss;
        ss << mllif::shared::Namespace + '.' + key;
        for (auto i = 0; i < valuesSize; ++i) {
            ss << Delimiter << values[i];
        }

        const auto attr = clang::AnnotateAttr::CreateImplicit(ctx, ss.str(), nullptr, 0);

        decl->addAttr(attr);
    }

    auto GetPath(const clang::NamedDecl *decl) -> std::vector<std::string> {
        std::vector<std::string> namespaces;
        for (; decl; decl = dyn_cast<clang::NamedDecl>(decl->getDeclContext())) {
            if (auto name = decl->getDeclName().getAsString(); !name.empty()) {
                namespaces.insert(namespaces.begin(), name);
            }
        }

        return namespaces;
    }
} // namespace

mllif::shared::Annotation::Annotation(const std::string &annotation) {
    bool key = true;
    std::stringstream ss{ annotation };
    for (std::string term; std::getline(ss, term, Delimiter); key = false) {
        if (key) {
            Key = term;
        } else {
            Values.emplace_back(term);
        }
    }
}

void mllif::shared::CreateAnnotation(clang::FunctionDecl *decl) {
    const auto isMethod = clang::dyn_cast<clang::CXXMethodDecl>(decl);
    const std::vector type = {(isMethod ? type::Method : type::Function)};
    ApplyAnnotation(decl, prefix::Type, type.data(), type.size());

    const auto dirs = GetPath(decl);
    ApplyAnnotation(decl, prefix::Path, dirs.data(), dirs.size());
}
