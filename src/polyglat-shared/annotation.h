#pragma once

#include <string>
#include <map>

namespace clang {
    class FunctionDecl;
} // namespace clang

namespace polyglat::shared {
    constexpr std::string Namespace = "$pg";

    namespace prefix {
        constexpr std::string Path = "path";
        constexpr std::string Parm = "parm";
        constexpr std::string Type = "type";
    }

    void CreateAnnotation(clang::FunctionDecl *decl);
} // namespace polyglat::shared
