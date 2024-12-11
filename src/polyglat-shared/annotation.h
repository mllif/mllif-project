#pragma once

#include <string>
#include <map>

namespace clang {
    class FunctionDecl;
} // namespace clang

namespace polyglat::shared {
    constexpr std::string Namespace = "__polyglat__";

#define LILAC_DEFINE_ANNOTATION_KEY(value) constexpr std::string value = #value
    // Use string rather than integer code to be human-readable
    LILAC_DEFINE_ANNOTATION_KEY(PATH);
    LILAC_DEFINE_ANNOTATION_KEY(NAME);
#undef LILAC_DEFINE_ANNOTATION_KEY

    void MarkAsTarget(clang::FunctionDecl *decl);
    void CreateAnnotation(clang::FunctionDecl *decl);
} // namespace polyglat::shared
