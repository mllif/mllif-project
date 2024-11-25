#pragma once

#include <optional>
#include <string>

namespace clang {
    class NamedDecl;
    class Attr;
} // namespace clang

namespace polyglat::shared {
    constexpr std::string Namespace = "__polyglat__";

#define LILAC_DEFINE_ANNOTATION_KEY(value) constexpr std::string value = #value
    // Use string rather than integer code to be human-readable
    LILAC_DEFINE_ANNOTATION_KEY(NAMESPACE);
    LILAC_DEFINE_ANNOTATION_KEY(NAME);
    LILAC_DEFINE_ANNOTATION_KEY(SIZE);
    LILAC_DEFINE_ANNOTATION_KEY(ALIGN);
    LILAC_DEFINE_ANNOTATION_KEY(CALLCONV);
    LILAC_DEFINE_ANNOTATION_KEY(RETURN);
    LILAC_DEFINE_ANNOTATION_KEY(TYPE);
#undef LILAC_DEFINE_ANNOTATION_KEY

    void MarkAsTarget(clang::NamedDecl *decl);
    void CreateAnnotation(clang::NamedDecl *decl);
    auto ParseAnnotation(const std::string &annotation) -> std::optional<std::pair<std::string, std::string>>;
} // namespace polyglat::shared
