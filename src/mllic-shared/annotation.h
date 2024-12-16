#pragma once

#include <string>
#include <map>

namespace clang {
    class FunctionDecl;
} // namespace clang

namespace mllic::shared {
    constexpr std::string Namespace = "$mllic";

    namespace prefix {
        constexpr std::string Path = "path";
        constexpr std::string Parm = "parm";
        constexpr std::string Type = "type";
    }

    void CreateAnnotation(clang::FunctionDecl *decl);
} // namespace mllic::shared
