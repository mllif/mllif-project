#pragma once

#include <string>

namespace clang {
    class FunctionDecl;
} // namespace clang

namespace mllif::shared {
    constexpr std::string Namespace = "$mllif";

    namespace prefix {
        constexpr std::string Path = "path";
        constexpr std::string Type = "type";
    } // namespace prefix

    namespace type {
        constexpr std::string Function = "function";
        constexpr std::string Method = "method";
        constexpr std::string Object = "object";
    } // namespace type

    /**
     * @brief Annotate a declaration with its information that may be lost
     * @param decl A declaration to be annotated
     */
    void CreateAnnotation(clang::FunctionDecl *decl);
} // namespace mllif::shared
