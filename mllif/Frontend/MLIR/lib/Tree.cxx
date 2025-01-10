#include "mllif/Frontend/MLIR/Tree.h"

void mllif::mlir::Node::print(llvm::raw_ostream &os) const {
    os << '<' << tag();

    if (name().size()) {
        os << " id=\"" << Escape(name()) << '"';
    }

    if (attributes().size()) {
        for (const auto &[key, value] : _attributes) {
            os << ' ' << key << "=\"" << Escape(value) << '"';
        }
    }

    if (children().empty()) {
        os << "/>";
        return;
    }

    os << '>';
    for (const auto &child : children()) {
        child.print(os);
    }
    os << "</" << tag() << '>';
}

mllif::mlir::Node *mllif::mlir::Node::insert_inplace(std::deque<std::string> &path, const std::string &tag) {
    if (path.empty()) {
        return nullptr;
    }

    for (auto &child : children()) {
        if (const auto p = child.insert(path, tag))
            return p;
    }

    auto &node = children().emplace_back(path.size() > 1 ? "namespace" : tag, path.front());
    path.pop_front();
    auto p = node.insert_inplace(path, tag);
    if (!p) {
        p = &node;
    }

    return p;
}

mllif::mlir::Node *mllif::mlir::Node::insert(std::deque<std::string> &path, const std::string &tag) {
    if (path.empty() || path.front() != name()) {
        return nullptr;
    }

    path.pop_front();
    if (path.empty()) {
        return this;
    }

    return insert_inplace(path, tag);
}
