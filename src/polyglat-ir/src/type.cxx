#include "polyglat-ir/type.h"

namespace {
    void replace(std::string &str, const std::string &from, const std::string &to) {
        if (from.empty()) {
            return;
        }

        for (
            size_t start_pos = 0;
            (start_pos = str.find(from, start_pos)) != std::string::npos;
            start_pos += to.length()) {
            str.replace(start_pos, from.length(), to);
        }
    }
} // namespace

polyglat::ir::Type::Type(const llvm::DataLayout &layout, llvm::Type *type)
    : Name(type->getStructName()),
      Size(layout.getTypeAllocSize(type)),
      Align(layout.getPrefTypeAlign(type)) {
    if (Name.starts_with("struct.")) {
        Name = Name.substr(7);
    }

    if (const auto pos = Name.rfind("::"); pos != std::string::npos) {
        Namespace = Name.substr(0, pos);
        replace(Namespace, "::", "/");
        Name = Name.substr(pos + 2);
    }

    if (!Namespace.starts_with('/')) {
        Namespace = '/' + Namespace;
    }
}

auto polyglat::ir::Type::ToString() const -> std::string {
    std::ostringstream ss;
    ss << "<type ns=\"" << Namespace
       << "\" name=\"" << Name
       << "\" size=\"" << Size
       << "\" align=\"" << Align.value()
       << "\"/>";
    return ss.str();
}
