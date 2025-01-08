#pragma once

#include <clang/Sema/ParsedAttr.h>

namespace mllif::c {

    class ExportAttrInfo final : public clang::ParsedAttrInfo {
      public:
        ExportAttrInfo();

        auto diagAppertainsToDecl(clang::Sema & /**/, const clang::ParsedAttr & /**/, const clang::Decl *D) const -> bool override;
        auto diagAppertainsToStmt(clang::Sema & /**/, const clang::ParsedAttr & /**/, const clang::Stmt * /**/) const -> bool override;
        auto handleDeclAttribute(clang::Sema &S, clang::Decl *D, const clang::ParsedAttr &/**/) const -> AttrHandling override;
    };
}
