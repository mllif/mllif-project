/*
 * Copyright 2025 Yeong-won Seo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mllif/Frontend/MLIR/Tree.h"

// NOLINTNEXTLINE(*-no-recursion)
auto mllif::mlir::Node::print(llvm::raw_ostream &os) const -> void {
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

// NOLINTNEXTLINE(*-no-recursion)
auto mllif::mlir::Node::insert_inplace(std::deque<std::string> &path, const std::string &tag) -> Node * {
    if (path.empty()) {
        return nullptr;
    }

    for (auto &child : children()) {
        if (const auto p = child.insert(path, tag)) {
            return p;
        }
    }

    auto &node = children().emplace_back(path.size() > 1 ? "namespace" : tag, path.front());
    path.pop_front();
    auto p = node.insert_inplace(path, tag);
    if (!p) {
        p = &node;
    }

    return p;
}

// NOLINTNEXTLINE(*-no-recursion)
auto mllif::mlir::Node::insert(std::deque<std::string> &path, const std::string &tag) -> Node * {
    if (path.empty() || path.front() != name()) {
        return nullptr;
    }

    path.pop_front();
    if (path.empty()) {
        return this;
    }

    return insert_inplace(path, tag);
}
