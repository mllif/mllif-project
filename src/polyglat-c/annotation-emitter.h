#pragma once

#include <clang/AST/RecursiveASTVisitor.h>

namespace polyglat::c {

    class AnnotationEmitter final : public clang::RecursiveASTVisitor<AnnotationEmitter> {
        clang::Sema *Sema;

        static auto ShouldBeAnnotated(const clang::NamedDecl *decl) -> bool;

      public:
        explicit AnnotationEmitter(clang::Sema *sema) : Sema(sema) {}

        auto VisitRecordDecl(clang::RecordDecl *decl) -> bool;

        auto VisitFunctionDecl(clang::FunctionDecl *decl) -> bool;
    };

    class ASTConsumer final : public clang::SemaConsumer {
        clang::Sema *Sema = nullptr;

      public:
        void HandleTranslationUnit(clang::ASTContext &ctx) override;

        void InitializeSema(clang::Sema &sema) override;
    };

    class AnnotationEmitAction final : public clang::PluginASTAction {
      public:
        auto CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef inFile) -> std::unique_ptr<clang::ASTConsumer> override;

        auto ParseArgs(const clang::CompilerInstance &ci, const std::vector<std::string> &arg) -> bool override;

        auto getActionType() -> ActionType override;
    };

} // namespace polyglat::c
