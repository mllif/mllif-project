#include "mllif/Frontend/MLIR/symboltree.h"

void mllif::mlir::Symbol::print(llvm::raw_ostream &ss, std::size_t indent) const {
    const std::string tab(indent, '\t');

    ss << tab << '<' << tag;
    for (const auto &[key, value] : attributes) {
        ss << ' ' << key << '=' << '"' << value << '"';
    }
    if (children.empty()) {
        ss << "/>\n";
        return;
    }

    ss << ">\n";
    for (auto child : children) {
        child.print(ss, indent + 1);
    }
    ss << tab << "</" << tag << ">\n";
}

mllif::mlir::RootSymbol::RootSymbol() : Symbol("assembly", "") {
}

void mllif::mlir::RootSymbol::print(llvm::raw_ostream &ss) const {
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    Symbol::print(ss);
}

mllif::mlir::Symbol &mllif::mlir::SymbolTree::insert(std::deque<std::string> path) {

    llvm::outs() << "PATH: ";
    for (const auto &token : path) {
        llvm::outs() << token << '/';
    }
    llvm::outs() << '\n';

    Symbol &scope = root;

    while (!path.empty()) {
        auto current = path.front();
        path.pop_front();

        const Symbol *p = nullptr;
        for (const auto &child : scope.children) {
            if (child.name == current) {
                p = &child;
            }
        }
        if (!p) {
            p = &scope.children.emplace_back("namespace", current);
        }

        scope = *p;
    }

    return scope;
}
void mllif::mlir::SymbolTree::print(llvm::raw_ostream &ss) const {
    root.print(ss);
}
