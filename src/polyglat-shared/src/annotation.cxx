#include "polyglat-shared/annotation.h"
#include "polyglat-shared/pch.h"

namespace {
    template<typename T>
    void ApplyAnnotation(clang::NamedDecl *decl, const std::string& key, const T &values) {

        const auto &ctx = decl->getASTContext();

        std::vector<clang::Expr *> args{values.size()};
        for (size_t i = 0; i < values.size(); ++i) {

            const auto arrayTy =
                ctx.getConstantArrayType(
                       ctx.CharTy,
                       llvm::APInt(ctx.getTypeSize(ctx.getSizeType()), values[i].size() + 1),
                       nullptr,
                       clang::ArraySizeModifier::Normal,
                       0)
                    .withConst();
            const auto decayTy = ctx.getPointerType(arrayTy);

            const auto literal =
                clang::StringLiteral::Create(
                    decl->getASTContext(),
                    values[i],
                    clang::StringLiteralKind::Ordinary,
                    false,
                    arrayTy,
                    {});
            const auto decay =
                clang::ImplicitCastExpr::Create(
                    decl->getASTContext(),
                    decayTy,
                    clang::CK_ArrayToPointerDecay, literal,
                    nullptr,
                    clang::VK_LValue,
                    {});

            args[i] = clang::ConstantExpr::Create(decl->getASTContext(), decay);
        }

        decl->addAttr(clang::AnnotateAttr::CreateImplicit(
            decl->getASTContext(),
            polyglat::shared::Namespace + '.' + key,
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

void polyglat::shared::CreateAnnotation(clang::FunctionDecl *decl) {
    const auto dirs = GetPath(decl);
    ApplyAnnotation(decl, prefix::Path, dirs);

    std::vector<std::string> params;
    params.reserve(decl->getNumParams());
    for (const auto parameter : decl->parameters()) {
        params.push_back(parameter->getNameAsString());
    }
    ApplyAnnotation(decl, prefix::Parm, params);

    const auto isMethod = clang::dyn_cast<clang::CXXMethodDecl>(decl);
    const std::vector<std::string> type = { isMethod ? "method" : "function" };
    ApplyAnnotation(decl, prefix::Type, type);
}
