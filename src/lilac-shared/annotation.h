#pragma once

#include <map>
#include <optional>
#include <string>

namespace clang {
    class NamedDecl;
    class Attr;
} // namespace clang

namespace lilac::shared {

#define LILAC_DEFINE_ANNOTATION_KEY(value) constexpr std::string value = #value
    // Use string rather than integer code to be human-readable
    LILAC_DEFINE_ANNOTATION_KEY(NAMESPACE);
    LILAC_DEFINE_ANNOTATION_KEY(NAME);
    LILAC_DEFINE_ANNOTATION_KEY(SIZE);
    LILAC_DEFINE_ANNOTATION_KEY(ALIGN);
    LILAC_DEFINE_ANNOTATION_KEY(CALLCONV);
    LILAC_DEFINE_ANNOTATION_KEY(RETURN);
#undef LILAC_DEFINE_ANNOTATION_KEY

    void CreateAnnotation(clang::NamedDecl *decl);
    auto ParseAnnotation(const std::string &annotation) -> std::optional<std::pair<std::string, std::string>>;
} // namespace lilac::shared
