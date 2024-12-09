#pragma once

namespace polyglat::ir {

    class Type {
        std::string Name;
        std::string Namespace;
        llvm::TypeSize Size;
        llvm::Align Align;

      public:
        explicit Type(const llvm::DataLayout& layout, llvm::Type *type);

        auto ToString() const -> std::string;
    };

} // namespace polyglat::ir
