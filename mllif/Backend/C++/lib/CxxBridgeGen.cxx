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

#include <mllif/Backend/C++/CxxBridgeGen.h>
#include <mllif/Backend/C++/Type.h>
#include <mllif/Backend/Context.h>

namespace mllif {
    auto Indent(size_t size) -> std::string {
        std::string indent(size * 4, ' ');
        return indent;
    }
}

bool mllif::cxx::CxxBridgeGen::handleAssemblyBegin(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) {
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleAssemblyEnd(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) {
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleNamespaceBegin(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) {
    out << Indent(indent) << "namespace " << node.name() << " {\n";
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleNamespaceEnd(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) {
    out << Indent(indent) << "} // namespace " << node.name() << '\n';
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleObjectBegin(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) {
    out << Indent(indent) << "class " << node.name() << " {\n"
        << Indent(indent + 1) << "char __data[" << node.size() << "];\n"
        << Indent(indent) << "public:\n";
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleObjectEnd(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) {
    out << Indent(indent) << "} __attribute((aligned(" << node.align() << ")));\n";
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleFunctionBegin(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) {
    const auto ret = TypeToCxx(node.returns());
    if (!ret) {
        context.error(std::format("unrecognized return type of function '{}'", node.symbol()));
        return false;
    }

    out << Indent(indent);
    if (std::dynamic_pointer_cast<ObjectDecl>(node.parent())) {
        out << "static ";
    }
    out << ret.value() << ' ' << node.name() << '(';

    return true;
}

bool mllif::cxx::CxxBridgeGen::handleFunctionEnd(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) {
    out << ") asm(\"" << node.symbol() << "\");\n";
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleMethodBegin(MLLIFContext &context, const MethodDecl &node, std::ostream &out, std::size_t indent) {
    const auto ret = TypeToCxx(node.returns());
    if (!ret) {
        context.error(std::format("unrecognized builtin type '{}'", node.returns().terms()[0]));
        return false;
    }

    out << Indent(indent) << ret.value() << ' ' << node.name() << '(';
    return true;
}

bool mllif::cxx::CxxBridgeGen::handleParam(MLLIFContext &context, const ParamDecl &node, std::ostream &out, std::size_t indent) {
    const auto type = TypeToCxx(node.type());
    if (!type) {
        context.error(std::format("unrecognized builtin type '{}'", node.type().terms()[0]));
        return false;
    }

    out << type.value() << ' ' << node.name();
    return true;
}

void mllif::cxx::CxxBridgeGen::writeParamDelimiter(std::ostream &os) {
    os << ", ";
}
