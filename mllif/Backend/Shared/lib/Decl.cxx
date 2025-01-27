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

#include "pch.h"

#include <cstring>
#include <mllif/Backend/Context.h>
#include <mllif/Backend/Decl.h>

mllif::Type::Type(MLLIFContext &context, std::string name) : _refs(0) {
    while (name.ends_with('*')) {
        name.erase(name.size() - 1);
        _refs++;
    }
    if (name.contains('*')) {
        context.error("identifier cannot contain '*' in middle of itself");
        return;
    }

    if (name.starts_with('/')) {
        name.erase(0, 1);
        std::stringstream ss(name);
        for (std::string term; std::getline(ss, term, '/');) {
            _terms.push_back(term);
        }
    } else {
        _builtin = true;
        _terms.push_back(name);
    }
}

mllif::Decl::Decl(MLLIFContext &context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : _parent(parent) {
    if (strcmp(node->name(), "assembly") == 0) {
        _name = "";
        return;
    }

    const auto name = node->first_attribute("id");
    if (!name) {
        context.error("identifier missing in declaration");
        return;
    }

    _name = name->value();
}

std::shared_ptr<mllif::Decl> mllif::Decl::Create(MLLIFContext &context, rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) {
#define NODE_HANDLER MLLIFContext &context, rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent
#define NODE_INIT(t) [](NODE_HANDLER) -> std::shared_ptr<Decl> { return std::make_shared<t##Decl>(context, node, parent); }
    std::map<std::string, std::shared_ptr<Decl> (*)(NODE_HANDLER)> handlers = {
        {"assembly", NODE_INIT(Assembly)},
        {"namespace", NODE_INIT(Namespace)},
        {"object", NODE_INIT(Object)},
        {"method", NODE_INIT(Method)},
        {"function", NODE_INIT(Function)},
        {"param", NODE_INIT(Param)}
    };
#undef NODE_INIT
#undef NODE_HANDLER

    std::cerr << node->name() << std::endl;

    if (!handlers.contains(node->name())) {
        context.error(std::format("unrecognized tag '{}'", node->name()));
        return nullptr;
    }

    auto decl = handlers[node->name()](context, node, parent);
    if (!context) {
        return decl;
    }

    for (auto child = node->first_node(); child; child = child->next_sibling()) {
        decl->_children.push_back(Create(context, child, parent));
    }

    return decl;
}

mllif::ObjectDecl::ObjectDecl(MLLIFContext &context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : Decl(context, node, parent) {
    const auto size = node->first_attribute("size");
    const auto align = node->first_attribute("align");

    _size = size ? size->value() : "1";
    _align = align ? align->value() : "1";
}

mllif::FunctionDecl::FunctionDecl(MLLIFContext &context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : Decl(context, node, parent) {
    const auto returns = node->first_attribute("ret");
    const auto sym = node->first_attribute("sym");

    if (!returns) {
        context.error(std::format("return type missing for function '{}'", name()));
    }
    if (!sym) {
        context.error(std::format("symbol name missing for function '{}'", name()));
    }
    if (!returns || !sym) {
        return;
    }

    _returns = Type(context, returns->value());
    _symbol = sym->value();
}

mllif::ParamDecl::ParamDecl(MLLIFContext &context, const rapidxml::xml_node<> *node, std::shared_ptr<Decl> parent) : Decl(context, node, parent) {
    const auto type = node->first_attribute("type");
    if (!type) {
        context.error(std::format("type missing for parameter '{}'", name()));
        return;
    }

    _type = Type(context, type->value());
}
