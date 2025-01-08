#include "pch.h"
#include "mllif/annotation.h"

namespace {
    template <typename T>
    void ApplyAnnotation(clang::NamedDecl *decl, const std::string &key, const T &values) {
        const auto &ctx = decl->getASTContext();

        std::vector<clang::Expr *> args;
        args.reserve(values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            const auto arrayTy =
                ctx.getConstantArrayType(
                       ctx.CharTy,
                       llvm::APInt(ctx.getTypeSize(ctx.getSizeType()), values[i].size() + 1),
                       nullptr,
                       clang::ArraySizeModifier::Normal,
                       0)
                    .withConst();

            const auto literal =
                clang::StringLiteral::Create(
                    decl->getASTContext(),
                    values[i],
                    clang::StringLiteralKind::Ordinary,
                    false,
                    arrayTy,
                    {});

            args.push_back(clang::ConstantExpr::Create(decl->getASTContext(), literal));
        }

        // Same size of args makes args copied... Why???
        decl->addAttr(clang::AnnotateAttr::CreateImplicit(
            decl->getASTContext(),
            mllif::shared::Namespace + '.' + key,
            args.data(),
            args.size()));
    }

    auto GetPath(const clang::NamedDecl *decl) -> std::deque<std::string> {
        std::deque<std::string> namespaces;
        for (; decl; decl = dyn_cast<clang::NamedDecl>(decl->getDeclContext())) {
            if (auto name = decl->getName(); !name.empty()) {
                namespaces.push_front(name.str());
            }
        }

        return namespaces;
    }
} // namespace

void mllif::shared::CreateAnnotation(clang::FunctionDecl *decl) {
    const auto isMethod = clang::dyn_cast<clang::CXXMethodDecl>(decl);
    const std::vector type = {(isMethod ? type::Method : type::Function)};
    ApplyAnnotation(decl, prefix::Type, type);

    const auto dirs = GetPath(decl);
    ApplyAnnotation(decl, prefix::Path, dirs);
}
