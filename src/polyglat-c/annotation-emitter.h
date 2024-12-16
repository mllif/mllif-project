#pragma once

#include <clang/AST/RecursiveASTVisitor.h>

namespace polyglat::c {
    /**
     * @brief A recursive-ast-visitor that emits annotations to symbols
     * to preserve object-oriented things over low-level environment (LLVM).
     */
    class AnnotationEmitter final : public clang::RecursiveASTVisitor<AnnotationEmitter> {
        clang::Sema *Sema;

        static auto ShouldBeAnnotated(const clang::NamedDecl *decl) -> bool;

      public:
        /**
         * @brief Creates new instance with specified semantic context.
         * @param sema A semantic context to use.
         */
        explicit AnnotationEmitter(clang::Sema *sema) : Sema(sema) {}

        /**
         * @brief Tries to annotate declaration of function.
         * @param decl A declaration to be annotated.
         */
        auto VisitFunctionDecl(clang::FunctionDecl *decl) -> bool;
    };

    /**
     * @brief A plugin-ast-action that emits annotations to result IR
     * to preserve object-oriented things over low-level environment (LLVM).
     */
    class AnnotationEmitAction final : public clang::PluginASTAction {
      public:
        /**
         * @brief Creates new instance.
         */
        auto CreateASTConsumer(clang::CompilerInstance &ci, llvm::StringRef inFile) -> std::unique_ptr<clang::ASTConsumer> override;

        auto ParseArgs(const clang::CompilerInstance &ci, const std::vector<std::string> &arg) -> bool override;

        auto getActionType() -> ActionType override;
    };

} // namespace polyglat::c
