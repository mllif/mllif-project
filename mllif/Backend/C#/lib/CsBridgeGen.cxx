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

#include <mllif/Backend/C#/CsBridgeGen.h>
#include <mllif/Backend/C#/Type.h>
#include <mllif/Backend/Context.h>

namespace mllif {
    auto Indent(size_t size) -> std::string {
        std::string indent(size * 4, ' ');
        return indent;
    }
} // namespace mllif

namespace mllif::cs {
    CsBridgeGen::CsBridgeGen(const std::string &libname) : LibraryName(libname) {
    }

    bool CsBridgeGen::handleAssemblyBegin(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) {
        return true;
    }
    bool CsBridgeGen::handleAssemblyEnd(MLLIFContext &context, const AssemblyDecl &node, std::ostream &out, std::size_t indent) {
        return true;
    }
    bool CsBridgeGen::handleNamespaceBegin(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) {
        out << Indent(indent) << "namespace " << node.name() << " {\n";
        return true;
    }
    bool CsBridgeGen::handleNamespaceEnd(MLLIFContext &context, const NamespaceDecl &node, std::ostream &out, std::size_t indent) {
        out << Indent(indent) << "} // namespace " << node.name() << '\n';
        return true;
    }
    bool CsBridgeGen::handleObjectBegin(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) {
        out << Indent(indent) << "[System.Runtime.InteropServices.StructLayout(LayoutKind.Explicit, Size=" << node.size() << ", Pack=" << node.align() << ")]\n"
            << Indent(indent) << "public struct " << node.name() << " {\n";
        return true;
    }
    bool CsBridgeGen::handleObjectEnd(MLLIFContext &context, const ObjectDecl &node, std::ostream &out, std::size_t indent) {
        out << Indent(indent) << "}\n";
        return true;
    }

    bool createFunctionStub(CsBridgeGen &gen, MLLIFContext &context, const FunctionDecl &node, std::ostream &out, const std::size_t indent) {

        const auto isMemberFn = std::dynamic_pointer_cast<ObjectDecl>(node.parent()) != nullptr;

        out << Indent(indent);

        if (isMemberFn) {
            // BEGIN fixed STATEMENT
            out << "fixed (void* __self = &this) { ";
        }

        if (const auto ret = node.returns(); !(ret.builtin() && ret.terms()[0] == "void")) {
            out << "return ";
        }

        out << "__stub("; // BEGIN STUB ARGUMENTS

        if (isMemberFn) {
            out << "__self, ";
        }

        for (auto i = 0; i < node.children().size(); i++) {
            if (i != 0) {
                out << ", ";
            }

            out << node.children()[i]->name();
        }

        auto ret = TypeToCs(node.returns());
        if (!ret) {
            context.error(std::format("unrecognized return type of function '{}'", node.symbol()));
            return false;
        }

        out << ");\n"; // END STUB ARGUMENTS

        if (isMemberFn) {
            // END fixed STATEMENT
            out << " }\n";
        }

        out << Indent(indent) << "[System.Runtime.InteropServices.DllImport(\"" << gen.LibraryName << "\", EntryPoint=\"" << node.symbol() << "\", ExactSpelling=true)]\n"
            << Indent(indent) << "static extern " << ret.value() << " __stub(";

        if (isMemberFn) {
            out << "void* __self, ";
        }

        for (auto i = 0; i < node.children().size(); i++) {
            if (i != 0) {
                gen.writeParamDelimiter(out);
            }

            gen.handleParam(context, *std::dynamic_pointer_cast<ParamDecl>(node.children()[i]), out, indent);
        }

        out << ");\n";

        return true;
    }

    bool CsBridgeGen::handleFunctionBegin(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) {
        const auto ret = TypeToCs(node.returns());
        if (!ret) {
            context.error(std::format("unrecognized return type of function '{}'", node.symbol()));
            return false;
        }

        if (!std::dynamic_pointer_cast<ObjectDecl>(node.parent())) {
            context.error(std::format("C# doesn't allow out-of-class functions ({})", node.symbol()));
            return false;
        }
        out << Indent(indent) << "public static " << ret.value() << ' ' << node.name() << '(';

        return true;
    }
    bool CsBridgeGen::handleFunctionEnd(MLLIFContext &context, const FunctionDecl &node, std::ostream &out, std::size_t indent) {
        out << ") {\n";
        out << Indent(indent + 1) << "unsafe {\n";
        createFunctionStub(*this, context, node, out, indent + 2);
        out << Indent(indent + 1) << "}\n";
        out << Indent(indent) << "}\n";
        return true;
    }
    bool CsBridgeGen::handleMethodBegin(MLLIFContext &context, const MethodDecl &node, std::ostream &out, std::size_t indent) {
        const auto ret = TypeToCs(node.returns());
        if (!ret) {
            context.error(std::format("unrecognized builtin type '{}'", node.returns().terms()[0]));
            return false;
        }

        out << Indent(indent) << "public " << ret.value() << ' ' << node.name() << '(';
        return true;
    }
    bool CsBridgeGen::handleParam(MLLIFContext &context, const ParamDecl &node, std::ostream &out, std::size_t indent) {
        const auto type = TypeToCs(node.type());
        if (!type) {
            context.error(std::format("unrecognized builtin type '{}'", node.type().terms()[0]));
            return false;
        }

        out << type.value() << ' ' << node.name();
        return true;
    }
    void CsBridgeGen::writeParamDelimiter(std::ostream &os) {
        os << ", ";
    }
} // namespace mllif::cs
