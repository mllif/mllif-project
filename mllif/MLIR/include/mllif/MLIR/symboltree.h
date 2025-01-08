#pragma once

#include <deque>

namespace mllif::mlir {

    class Symbol {
        void print(llvm::raw_ostream &ss, std::size_t indent) const;

    public:
        virtual ~Symbol() = default;

        Symbol(const std::string &tag, const std::string &name) : tag(tag), name(name) {}

        std::string tag;
        std::string name;

        std::vector<std::pair<std::string, std::string>> attributes;
        std::vector<Symbol> children;

        virtual void print(llvm::raw_ostream &ss) const {
            print(ss, 0);
        }
    };

    class RootSymbol final : public Symbol {
    public:
        RootSymbol();

        void print(llvm::raw_ostream &ss) const override;
    };

    class SymbolTree {
        RootSymbol root;

    public:
        Symbol& insert(std::deque<std::string> path);

        void print(llvm::raw_ostream &ss) const;
    };

} // namespace mllif::mlir
