#include <clang/AST/Decl.h>
#include <sstream>
#include <stack>
#include <utility>

#include "lilac-shared/annotation.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>

namespace {
    constexpr std::string NS = "__lilac_ns__";

    auto CreateAnnotation(const std::string &key, const std::string &value) -> std::string {
        std::stringstream ss;
        ss << NS << key << '\x02' << value;
        return ss.str();
    }

    void ApplyAnnotation(clang::NamedDecl *decl, const std::string &value) {
        const auto attr = clang::AnnotateAttr::Create(decl->getASTContext(), value, nullptr, 0);
        decl->addAttr(attr);
    }

    auto GetNamespace(const clang::NamedDecl *decl) -> std::string {
        std::stack<std::string> namespaces;
        for (
            decl = dyn_cast<clang::NamespaceDecl>(decl->getDeclContext());
            decl;
            decl = dyn_cast<clang::NamespaceDecl>(decl->getDeclContext())) {
            if (auto name = decl->getQualifiedNameAsString(); !name.empty()) {
                namespaces.push(decl->getNameAsString());
            }
        }

        std::stringstream ss;
        for (; !namespaces.empty(); namespaces.pop()) {
            ss << '/' << namespaces.top();
        }

        return ss.str();
    }

    class TypeID {
        std::string Namespace;
        std::string Name;
        size_t RefCnt;

      public:
        TypeID(std::string ns, std::string name, const size_t refCnt)
            : Namespace(std::move(ns)),
              Name(std::move(name)),
              RefCnt(refCnt) {
        }

        [[nodiscard]]
        auto str() const -> std::string {
            std::stringstream ss;
            ss << Namespace << "/" << Name << ',' << RefCnt;
            return ss.str();
        }
    };

    auto GetTypeID(const clang::Type *type) -> TypeID {
        size_t refCnt = 0;
        while (true) {
            if (const auto pointee = type->getPointeeType().getTypePtrOrNull()) {
                type = pointee;
                refCnt++;
                continue;
            }

            if (const auto array = dyn_cast<clang::ArrayType>(type)) {
                type = array->getArrayElementTypeNoTypeQual();
                refCnt++;
                continue;
            }

            break;
        }

        const auto decl = type->getAsRecordDecl();
        return {
            .Namespace = GetNamespace(decl),
            .Name = decl->getNameAsString(),
            .RefCnt = refCnt,
        };
    }

    auto ParseTypeID(const std::string &id) -> std::optional<TypeID> {
        constexpr int BASE10 = 10;

        auto delimiter = id.rfind('/');
        if (delimiter == std::string::npos) {
            return std::nullopt;
        }

        const auto ns = id.substr(0, delimiter);
        auto name = id.substr(delimiter + 1);

        delimiter = name.find(',');
        const auto refCntStr = name.substr(delimiter + 1);
        name = name.substr(0, delimiter);

        const auto refCnt = std::strtoul(refCntStr.c_str(), nullptr, BASE10);

        return TypeID{
            .Namespace = ns,
            .Name = name,
            .RefCnt = refCnt,
        };
    }

    auto GetCallConv(const clang::FunctionDecl *fn) -> std::string {
        const auto cc = fn->getFunctionType()->getCallConv();
        return clang::FunctionType::getNameForCallConv(cc).str();
    }
} // namespace

void lilac::shared::CreateAnnotation(clang::NamedDecl *decl) {
    if (const auto typeDecl = dyn_cast<clang::TypeDecl>(decl)) {
        constexpr size_t BIT_PER_BYTE = 8;

        const auto info = typeDecl->getASTContext().getTypeInfo(typeDecl->getTypeForDecl());
        ApplyAnnotation(decl, ::CreateAnnotation(NAMESPACE, GetNamespace(decl)));
        ApplyAnnotation(decl, ::CreateAnnotation(NAME, decl->getNameAsString()));
        ApplyAnnotation(decl, ::CreateAnnotation(SIZE, std::to_string(info.Width / BIT_PER_BYTE)));
        ApplyAnnotation(decl, ::CreateAnnotation(ALIGN, std::to_string(info.Align / BIT_PER_BYTE)));
    } else if (const auto funcDecl = dyn_cast<clang::FunctionDecl>(decl)) {
        ApplyAnnotation(decl, ::CreateAnnotation(NAMESPACE, GetNamespace(decl)));
        ApplyAnnotation(decl, ::CreateAnnotation(NAME, decl->getNameAsString()));
        ApplyAnnotation(decl, ::CreateAnnotation(CALLCONV, GetCallConv(funcDecl)));
        ApplyAnnotation(decl, ::CreateAnnotation(RETURN, GetTypeID(funcDecl->getReturnType().getTypePtr()).str()));
    }
}

auto lilac::shared::ParseAnnotation(const std::string &annotation) -> std::optional<std::pair<std::string, std::string>> {
    if (!annotation.starts_with(NS)) {
        return std::nullopt;
    }

    const auto line = annotation.substr(NS.size() + 1);
    const auto delimiter = line.find('\x02');

    return std::make_pair(line.substr(0, delimiter), line.substr(delimiter + 1));
}
