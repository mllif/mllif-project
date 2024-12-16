
#include "polyglat-c/annotation-emitter.h"
#include "polyglat-c/pch.h"

#include <polyglat-shared/annotation.h>

auto polyglat::c::AnnotationEmitter::ShouldBeAnnotated(const clang::NamedDecl *decl) -> bool {
    auto target = false;
    for (const auto attr : decl->attrs()) {
        if (const auto annotationAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
            target = annotationAttr->getAnnotation() == shared::Namespace;
        }
        if (target) {
            break;
        }
    }
    return target;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppDFAConstantFunctionResult
auto polyglat::c::AnnotationEmitter::VisitFunctionDecl(clang::FunctionDecl *decl) -> bool {
    if (!ShouldBeAnnotated(decl)) {
        return true;
    }

    shared::CreateAnnotation(decl);

    return true;
}

class ASTConsumer final : public clang::SemaConsumer {
    clang::Sema *Sema = nullptr;

  public:
    void HandleTranslationUnit(clang::ASTContext &ctx) override {
        polyglat::c::AnnotationEmitter emitter{Sema};
        emitter.TraverseDecl(ctx.getTranslationUnitDecl());
    }

    void InitializeSema(clang::Sema &sema) override {
        Sema = &sema;
    }
};

auto polyglat::c::AnnotationEmitAction::CreateASTConsumer(clang::CompilerInstance & /**/, llvm::StringRef /**/) -> std::unique_ptr<clang::ASTConsumer> {
    return std::make_unique<ASTConsumer>();
}

auto polyglat::c::AnnotationEmitAction::ParseArgs(const clang::CompilerInstance & /**/, const std::vector<std::string> & /**/) -> bool {
    return true;
}

auto polyglat::c::AnnotationEmitAction::getActionType() -> ActionType {
    return AddBeforeMainAction;
}

namespace {
    [[maybe_unused]]
    // NOLINTNEXTLINE(cert-err58-cpp) : Initialization of 'X' with static storage duration may throw an exception that cannot be caught
    const clang::FrontendPluginRegistry::Add<polyglat::c::AnnotationEmitAction> X("polyglat", "");
}
