#pragma once

#include <deque>

namespace mllif::mlir {

    class Node final {
        std::string _tag;
        std::string _name;
        std::vector<std::pair<std::string, std::string>> _attributes;
        std::vector<Node> _children;

        static std::string Escape(std::string s) {
            std::stringstream ss;
            for (const auto c : s) {
                if (!std::isprint(c)) {
                    ss << "&#" << static_cast<int>(c) << ';';
                }

                switch (c) {
                case '"':
                    ss << "&quot;";
                    break;
                case '\'':
                    ss << "&apos;";
                    break;
                case '<':
                    ss << "&lt;";
                    break;
                case '>':
                    ss << "&gt;";
                    break;
                case '&':
                    ss << "&amp;";
                    break;
                default:
                    ss << c;
                    break;
                }
            }
            return ss.str();
        }

      public:
        explicit Node(std::string tag, std::string name) : _tag(std::move(tag)), _name(std::move(name)) {}

        std::string &tag() { return _tag; }
        std::string tag() const { return _tag; }

        std::string name() const { return _name; }

        std::vector<std::pair<std::string, std::string>> &attributes() { return _attributes; }
        const std::vector<std::pair<std::string, std::string>> &attributes() const { return _attributes; }

        std::vector<Node> &children() { return _children; }
        const std::vector<Node> &children() const { return _children; }

        void print(llvm::raw_ostream &os) const {
            os << '<' << tag();

            if (name().size()) {
                os << " id=\"" << Escape(name()) << '"';
            }

            if (attributes().size()) {
                for (const auto &[key, value] : _attributes) {
                    os << ' ' << key << "=\"" << Escape(value) << '"';
                }
            }

            if (children().size()) {
                os << "/>";
                return;
            }

            os << '>';
            for (const auto &child : children()) {
                child.print(os);
            }
            os << "</" << tag() << '>';
        }

        Node *insert_inplace(std::deque<std::string> &path, const std::string &tag) {
            if (path.empty()) {
                return nullptr;
            }

            for (auto &child : children()) {
                if (const auto p = child.insert(path, tag))
                    return p;
            }

            auto &node = children().emplace_back(path.size() > 1 ? "namespace" : tag, path.front());
            path.pop_front();
            node.insert_inplace(path, tag);

            return &node;
        }

        Node *insert(std::deque<std::string> &path, const std::string &tag) {
            if (path.empty() || path.front() != name()) {
                return nullptr;
            }

            path.pop_front();
            if (path.empty()) {
                return this;
            }

            return insert_inplace(path, tag);
        }
    };

    class Tree {
        Node _root;

      public:
        Tree() : _root("assembly", "") {}

        Node &root() { return _root; }
    };

} // namespace mllif::mlir
