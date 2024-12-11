#include "polyglat-shared/pch.h"
#include "polyglat-shared/annotation.h"

namespace {
    void ApplyAnnotation(clang::NamedDecl *decl, const std::vector<std::string>& values) {

        const auto & ctx = decl->getASTContext();

        std::vector<clang::Expr*> args{ values.size() };
        for (size_t i = 0; i < values.size(); ++i) {
            const auto arrayTy = ctx.getConstantArrayType(
                ctx.Char8Ty,
                llvm::APInt(ctx.getTypeSize(ctx.getSizeType()), values[i].size() + 1),
                nullptr,
                clang::ArraySizeModifier::Normal,
                0);
            args[i] = clang::StringLiteral::Create(
                decl->getASTContext(),
                values[i],
                clang::StringLiteralKind::UTF8,
                false,
                arrayTy,
                clang::SourceLocation());
        }

        decl->addAttr(clang::AnnotateAttr::CreateImplicit(decl->getASTContext(), polyglat::shared::Namespace, args.data(), args.size()));
    }

    auto GetDirectory(const clang::NamedDecl *decl) -> std::vector<std::string> {
        std::vector<std::string> namespaces;
        for (
            decl = dyn_cast<clang::NamedDecl>(decl->getDeclContext());
            decl;
            decl = dyn_cast<clang::NamedDecl>(decl->getDeclContext())) {
            if (auto name = decl->getName(); !name.empty()) {
                namespaces.push_back(name.str());
            }
        }

        return namespaces;
    }
} // namespace

void polyglat::shared::MarkAsTarget(clang::FunctionDecl *decl) {
    ApplyAnnotation(decl, { Namespace });
}

void polyglat::shared::CreateAnnotation(clang::FunctionDecl *decl) {
    auto dirs = GetDirectory(decl);
    dirs.insert(dirs.begin(), PATH);
    ApplyAnnotation(decl, dirs);
    ApplyAnnotation(decl, { NAME, decl->getNameAsString() });
}
