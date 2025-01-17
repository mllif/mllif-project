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

#pragma once

#include <deque>

namespace mllif::mlir {

    /**
     * @brief A node for symbol tree. It represents just simple XML serializer.
     */
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
        /**
         * @brief Creates new node
         * @param tag Tag of node
         * @param name Name (id attribute) of node
         */
        explicit Node(std::string tag, std::string name) : _tag(std::move(tag)), _name(std::move(name)) {}

        /**
         * @brief Gets a tag of node
         * @return Tag of node
         */
        std::string &tag() { return _tag; }

        /**
         * @brief Gets a tag of node as readonly
         * @return Tag of node
         */
        std::string tag() const { return _tag; }

        /**
         * @brief Gets a name of node
         * @return Name of node
         */
        std::string &name() { return _name; }

        /**
         * @brief Gets a name of node as readonly
         * @return Name of node
         */
        std::string name() const { return _name; }

        /**
         * @brief Gets attributes of node
         * @return Attributes of node
         */
        std::vector<std::pair<std::string, std::string>> &attributes() { return _attributes; }

        /**
         * @brief Gets attributes of node as readonly
         * @return Attributes of node
         */
        const std::vector<std::pair<std::string, std::string>> &attributes() const { return _attributes; }

        /**
         * @brief Gets children of node
         * @return Children of node
         */
        std::vector<Node> &children() { return _children; }

        /**
         * @brief Gets children of node as readonly
         * @return Children of node
         */
        const std::vector<Node> &children() const { return _children; }

        /**
         * @brief Serialize subtree as XML to stream
         * @return Children of node
         */
        void print(llvm::raw_ostream &os) const;

        /**
         * @brief Inserts new node in-place at given path (first element of path is not id of this node)
         * @param path Path of new node
         * @param tag Tag of new node (tag of missing will be `namespace`)
         * @return Created node (If path is already used by other node, that node will be return; Given arguments ignored)
         * @retval nullptr if path is empty
         */
        Node *insert_inplace(std::deque<std::string> &path, const std::string &tag);

        /**
         * @brief Inserts new node at given path (first element of path is id of this node)
         * @param path Path of new node
         * @param tag Tag of new node (tag of missing will be `namespace`)
         * @return Created node (If path is already used by other node, that node will be return; Given arguments ignored)
         * @retval nullptr if path is empty or path's first element is not id of this node
         */
        Node *insert(std::deque<std::string> &path, const std::string &tag);
    };

    /**
     * @brief A tree struct for symbol tree. It's just a simple wrapper for root node.
     */
    class Tree {
        Node _root;

      public:
        /**
         * @brief Creates new tree
         */
        Tree() : _root("assembly", "") {}

        /**
         * @brief Gets a root node of the tree
         * @return Root node of the tree
         */
        Node &root() { return _root; }
    };

} // namespace mllif::mlir
