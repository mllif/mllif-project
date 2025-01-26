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

#include <vector>
#include <memory>
#include <rapidxml/rapidxml.hpp>

namespace mllif {
    class MLLIFContext;

    class Type {
        std::vector<std::string> _terms;
        std::size_t _refs;
        bool _builtin;

      public:
        Type() = default;
        Type(MLLIFContext& context, std::string name);

        const std::vector<std::string>& terms() const { return _terms; }
        std::size_t refs() const { return _refs; }
        bool builtin() const { return _builtin; }
    };

    class Decl {
        std::string _name;
        std::vector<std::shared_ptr<Decl>> _children;
        std::shared_ptr<Decl> _parent;

      public:
        Decl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent);
        virtual ~Decl() = default;

        const std::string& name() const { return _name; }
        const std::vector<std::shared_ptr<Decl>>& children() const { return _children; }
        const std::shared_ptr<Decl>& parent() const { return _parent; }

        static std::shared_ptr<Decl> Create(MLLIFContext& context, rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent);
    };

    class AssemblyDecl final : public Decl {
      public:
        AssemblyDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : Decl(context, node, parent) {};
    };

    class NamespaceDecl final : public Decl {
      public:
        NamespaceDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : Decl(context, node, parent) {};
    };

    class ObjectDecl final : public Decl {
        std::string _size;
        std::string _align;

      public:
        ObjectDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent);

        const std::string& size() const { return _size; }
        const std::string& align() const { return _align; }
    };

    class FunctionDecl : public Decl {
        Type _returns;
        std::string _symbol;

      public:
        FunctionDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent);

        const Type& returns() const { return _returns; }
        const std::string& symbol() const { return _symbol; }
    };

    class MethodDecl final : public FunctionDecl {
      public:
        MethodDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : FunctionDecl(context, node, parent) {};
    };

    class ParamDecl final : public Decl {
        Type _type;

      public:
        ParamDecl(MLLIFContext& context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent);

        const Type& type() const { return _type; }
    };
}
